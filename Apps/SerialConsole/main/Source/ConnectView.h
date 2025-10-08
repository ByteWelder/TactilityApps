#pragma once

#include "View.h"

#include <string>
#include <vector>
#include <lvgl.h>
#include <Str.h>
#include <functional>
#include <memory>

#include <tt_app_alertdialog.h>
#include <tt_hal_uart.h>
#include <tt_lvgl.h>
#include <TactilityCpp/LvglLock.h>
#include <TactilityCpp/Uart.h>
#include <TactilityCpp/Preferences.h>

class ConnectView final : public View {

public:

    typedef std::function<void(std::unique_ptr<Uart>)> OnConnectedFunction;
    std::vector<Str> uartNames;
    Preferences preferences = Preferences("SerialConsole");
    LvglLock lvglLock;

private:

    OnConnectedFunction onConnected;
    lv_obj_t* busDropdown = nullptr;
    lv_obj_t* speedTextarea = nullptr;

    Str join(const std::vector<Str>& list) {
        Str output;
        for (int i = list.size() - 1; i >= 0; i--) {
            output.append(list[i].c_str());
            if (i < list.size() - 1) {
                output.append(",");
            }
        }
        return output;
    }

    int32_t getSpeedInput() const {
        auto* speed_text = lv_textarea_get_text(speedTextarea);
        return atoi(speed_text);
    }

    void onConnect() {
        auto lock = lvglLock.asScopedLock();
        if (!lock.lock(TT_LVGL_DEFAULT_LOCK_TIME)) {
            return;
        }

        const char* alert_dialog_labels[] = { "OK" };

        auto selected_uart_index = lv_dropdown_get_selected(busDropdown);
        if (selected_uart_index >= uartNames.size()) {
            tt_app_alertdialog_start("Error", "No UART selected", alert_dialog_labels, 1);
            return;
        }

        auto uart = Uart::open(selected_uart_index);
        if (uart == nullptr) {
            tt_app_alertdialog_start("Error", "Failed to connect to UART", alert_dialog_labels, 1);
            return;
        }

        int speed = getSpeedInput();
        if (speed <= 0) {
            tt_app_alertdialog_start("Error", "Invalid speed", alert_dialog_labels, 1);
            return;
        }

        if (!uart->start()) {
            tt_app_alertdialog_start("Error", "Failed to initialize", alert_dialog_labels, 1);
            return;
        }

        if (!uart->setBaudRate(speed)) {
            uart->stop();
            tt_app_alertdialog_start("Error", "Failed to set baud rate", alert_dialog_labels, 1);
            return;
        }

        onConnected(std::move(uart));
    }

    static void onConnectCallback(lv_event_t* event) {
        auto* view = static_cast<ConnectView*>(lv_event_get_user_data(event));
        view->onConnect();
    }

    static lv_obj_t* createRowWrapper(lv_obj_t* parent) {
        auto* wrapper = lv_obj_create(parent);
        lv_obj_set_size(wrapper, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_border_width(wrapper, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(wrapper, 0, LV_STATE_DEFAULT);
        return wrapper;
    }

public:

    explicit ConnectView(OnConnectedFunction onConnected) : onConnected(std::move(onConnected)) {}

    void onStart(lv_obj_t* parent) {
        uartNames = Uart::getNames();

        auto* wrapper = lv_obj_create(parent);
        lv_obj_set_flex_flow(wrapper, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_size(wrapper, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_border_width(wrapper, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(wrapper, 0, LV_STATE_DEFAULT);

        // Bus selection

        auto* bus_wrapper = createRowWrapper(wrapper);

        busDropdown = lv_dropdown_create(bus_wrapper);

        auto bus_options = join(uartNames);
        lv_dropdown_set_options(busDropdown, bus_options.c_str());
        lv_obj_align(busDropdown, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_set_width(busDropdown, LV_PCT(50));

        int32_t bus_index = 0;
        preferences.optInt32("bus", bus_index);
        if (bus_index < uartNames.size()) {
            lv_dropdown_set_selected(busDropdown, bus_index);
        }

        auto* bus_label = lv_label_create(bus_wrapper);
        lv_obj_align(bus_label, LV_ALIGN_LEFT_MID, 0, 0);
        lv_label_set_text(bus_label, "Bus");

        // Baud rate selection
        auto* baud_wrapper = createRowWrapper(wrapper);

        int32_t speed = 115200;
        preferences.optInt32("speed", speed);
        speedTextarea = lv_textarea_create(baud_wrapper);
        lv_textarea_set_text(speedTextarea, std::to_string(speed).c_str());
        lv_textarea_set_one_line(speedTextarea, true);
        lv_obj_set_width(speedTextarea, LV_PCT(50));
        lv_obj_align(speedTextarea, LV_ALIGN_TOP_RIGHT, 0, 0);

        auto* baud_rate_label = lv_label_create(baud_wrapper);
        lv_obj_align(baud_rate_label, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_label_set_text(baud_rate_label, "Baud");

        // Connect
        auto* connect_wrapper = createRowWrapper(wrapper);

        auto* connect_button = lv_button_create(connect_wrapper);
        lv_obj_align(connect_button, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(connect_button, onConnectCallback, LV_EVENT_SHORT_CLICKED, this);
        auto* connect_label = lv_label_create(connect_button);
        lv_label_set_text(connect_label, "Connect");
    }

    void onStop() override {
        int speed = getSpeedInput();
        if (speed > 0) {
            preferences.putInt32("speed", speed);
        }

        auto bus_index = static_cast<int32_t>(lv_dropdown_get_selected(busDropdown));
        preferences.putInt32("bus", bus_index);
    }
};
