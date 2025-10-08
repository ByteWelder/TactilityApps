#pragma once

#include "View.h"
#include "esp_log.h"

#include <Str.h>
#include <sstream>
#include <lvgl.h>
#include <memory>

#include <tt_lvgl.h>
#include <tt_thread.h>

#include <TactilityCpp/Mutex.h>
#include <TactilityCpp/Thread.h>
#include <TactilityCpp/LvglLock.h>

constexpr size_t receiveBufferSize = 512;
constexpr size_t renderBufferSize = receiveBufferSize + 2; // Leave space for newline at split and null terminator at the end

class ConsoleView final : public View {

    const char* TAG = "SerialConsole";

    lv_obj_t* _Nullable parent = nullptr;
    lv_obj_t* _Nullable logTextarea = nullptr;
    lv_obj_t* _Nullable inputTextarea = nullptr;
    std::shared_ptr<Uart> _Nullable uart = nullptr;
    std::shared_ptr<Thread> uartThread _Nullable = nullptr;
    bool uartThreadInterrupted = false;
    std::shared_ptr<Thread> viewThread _Nullable = nullptr;
    bool viewThreadInterrupted = false;
    Mutex mutex = Mutex(MutexTypeRecursive);
    uint8_t receiveBuffer[receiveBufferSize];
    uint8_t renderBuffer[renderBufferSize];
    size_t receiveBufferPosition = 0;
    Str terminatorString = "\n";

    LvglLock lvglLock;

    bool isUartThreadInterrupted() const {
        auto lock = mutex.asScopedLock();
        lock.lock();
        return uartThreadInterrupted;
    }

    bool isViewThreadInterrupted() const {
        auto lock = mutex.asScopedLock();
        lock.lock();
        return viewThreadInterrupted;
    }

    void updateViews() {
        auto scoped_lvgl_lock = lvglLock.asScopedLock();
        if (!scoped_lvgl_lock.lock()) {
            return;
        }

        if (parent == nullptr) {
            return;
        }

        // Updating the view is expensive, so we only want to set the text once:
        // Gather all the lines in a single buffer
        if (mutex.lock()) {
            size_t first_part_size = receiveBufferSize - receiveBufferPosition;
            memcpy(renderBuffer, receiveBuffer + receiveBufferPosition, first_part_size);
            renderBuffer[receiveBufferPosition] = '\n';
            if (receiveBufferPosition > 0) {
                memcpy(renderBuffer + first_part_size + 1, receiveBuffer, (receiveBufferSize - first_part_size));
                renderBuffer[receiveBufferSize - 1] = 0x00;
            }
            mutex.unlock();
        }

        tt_lvgl_lock(TT_MAX_TICKS);
        lv_textarea_set_text(logTextarea, (const char*)renderBuffer);
        tt_lvgl_unlock();
    }

    int32_t viewThreadMain() {
        while (!isViewThreadInterrupted()) {
            auto start_time = tt_kernel_get_ticks();

            updateViews();

            auto end_time = tt_kernel_get_ticks();
            auto time_diff = end_time - start_time;
            if (time_diff < 500U) {
                tt_kernel_delay_ticks((500U - time_diff) / portTICK_PERIOD_MS);
            }
        }

        return 0;
    }

    static int32_t viewThreadMainStatic(void* context) {
        auto* self = static_cast<ConsoleView*>(context);
        return self->viewThreadMain();
    }

    int32_t uartThreadMain() {
        char byte;

        while (!isUartThreadInterrupted()) {
            assert(uart != nullptr);
            bool success = uart->readByte(&byte, 50 / portTICK_PERIOD_MS);

            // Thread might've been interrupted in the meanwhile
            if (isUartThreadInterrupted()) {
                break;
            }

            if (success) {
                mutex.lock();
                receiveBuffer[receiveBufferPosition++] = byte;
                if (receiveBufferPosition == receiveBufferSize) {
                    receiveBufferPosition = 0;
                }
                mutex.unlock();
            }

        }

        return 0;
    }

    static int32_t uartThreadMainStatic(void* view) {
        auto* self = static_cast<ConsoleView*>(view);
        return self->uartThreadMain();
    }

    static void onSendClickedCallback(lv_event_t* event) {
        auto* view = (ConsoleView*)lv_event_get_user_data(event);
        view->onSendClicked();
    }

    static void onTerminatorDropdownValueChangedCallback(lv_event_t* event) {
        auto* view = (ConsoleView*)lv_event_get_user_data(event);
        view->onTerminatorDropDownValueChanged(event);
    }

    void onTerminatorDropDownValueChanged(lv_event_t* event) {
        auto* dropdown = static_cast<lv_obj_t*>(lv_event_get_target(event));
        mutex.lock();
        switch (lv_dropdown_get_selected(dropdown)) {
            case 0:
                terminatorString = "\n";
                break;
            case 1:
                terminatorString = "\r\n";
                break;
        }
        mutex.unlock();
    }

    void onSendClicked() {
        mutex.lock();
        Str input_text = lv_textarea_get_text(inputTextarea);
        Str to_send;
        to_send.appendf("%s%s", input_text.c_str(), terminatorString.c_str());
        mutex.unlock();

        if (uart != nullptr) {
            if (!uart->writeBytes(to_send.c_str(), to_send.length(), 100 / portTICK_PERIOD_MS)) {
                ESP_LOGE(TAG, "Failed to send \"%s\"", input_text.c_str());
            }
        }

        lv_textarea_set_text(inputTextarea, "");
    }

public:

    void startLogic(std::unique_ptr<Uart> newUart) {
        memset(receiveBuffer, 0, receiveBufferSize);

        assert(uartThread == nullptr);
        assert(uart == nullptr);

        uart = std::move(newUart);

        uartThreadInterrupted = false;
        uartThread = std::make_unique<Thread>(
            "SerConsUart",
            4096,
            uartThreadMainStatic,
            this
        );
        uartThread->setPriority(ThreadPriorityHigh);
        uartThread->start();
    }

    void startViews(lv_obj_t* parent) {
        this->parent = parent;

        lv_obj_set_style_pad_gap(parent, 2, 0);

        logTextarea = lv_textarea_create(parent);
        lv_textarea_set_placeholder_text(logTextarea, "Waiting for data...");
        lv_obj_set_flex_grow(logTextarea, 1);
        lv_obj_set_width(logTextarea, LV_PCT(100));
        lv_obj_add_state(logTextarea, LV_STATE_DISABLED);
        lv_obj_set_style_margin_ver(logTextarea, 0, 0);

        auto* input_wrapper = lv_obj_create(parent);
        lv_obj_set_size(input_wrapper, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(input_wrapper, 0, 0);
        lv_obj_set_style_border_width(input_wrapper, 0, 0);
        lv_obj_set_width(input_wrapper, LV_PCT(100));
        lv_obj_set_flex_flow(input_wrapper, LV_FLEX_FLOW_ROW);

        inputTextarea = lv_textarea_create(input_wrapper);
        lv_textarea_set_one_line(inputTextarea, true);
        lv_textarea_set_placeholder_text(inputTextarea, "Text to send");
        lv_obj_set_width(inputTextarea, LV_PCT(100));
        lv_obj_set_flex_grow(inputTextarea, 1);

        auto* terminator_dropdown = lv_dropdown_create(input_wrapper);
        lv_dropdown_set_options(terminator_dropdown, "\\n\n\\r\\n");
        lv_obj_set_width(terminator_dropdown, 70);
        lv_obj_add_event_cb(terminator_dropdown, onTerminatorDropdownValueChangedCallback, LV_EVENT_VALUE_CHANGED, this);


        auto* button = lv_button_create(input_wrapper);
        auto* button_label = lv_label_create(button);
        lv_label_set_text(button_label, "Send");
        lv_obj_add_event_cb(button, onSendClickedCallback, LV_EVENT_SHORT_CLICKED, this);

        viewThreadInterrupted = false;
        viewThread = std::make_unique<Thread>(
            "SerConsView",
            4096,
            viewThreadMainStatic,
            this
        );
        viewThread->setPriority(ThreadPriorityHigher);
        viewThread->start();
    }

    void stopLogic() {
        auto lock = mutex.asScopedLock();
        lock.lock();

        uartThreadInterrupted = true;

        // Detach thread, it will auto-delete when leaving the current scope
        auto old_uart_thread = std::move(uartThread);
        // Unlock so thread can lock
        lock.unlock();

        if (old_uart_thread->getState() != ThreadStateStopped) {
            // Wait for thread to finish
            old_uart_thread->join();
        }
    }

    void stopViews() {
        auto lock = mutex.asScopedLock();
        lock.lock();

        viewThreadInterrupted = true;

        // Detach thread, it will auto-delete when leaving the current scope
        auto old_view_thread = std::move(viewThread);

        // Unlock so thread can lock
        lock.unlock();

        if (old_view_thread->getState() != ThreadStateStopped) {
            // Wait for thread to finish
            old_view_thread->join();
        }
    }

    void stopUart() {
        auto lock = mutex.asScopedLock();
        lock.lock();

        if (uart != nullptr && uart->isStarted()) {
            uart->stop();
            uart = nullptr;
        }
    }

    void onStart(lv_obj_t* parent, std::unique_ptr<Uart> newUart) {
        auto lock = mutex.asScopedLock();
        lock.lock();

        startLogic(std::move(newUart));
        startViews(parent);
    }

    void onStop() override {
        stopViews();
        stopLogic();
        stopUart();
    }
};
