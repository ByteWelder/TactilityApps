#include "Application.h"

#include <tt_app_alertdialog.h>
#include <tt_hal.h>
#include <tt_hal_gpio.h>
#include <tt_lvgl.h>
#include <tt_lvgl_toolbar.h>

#include <esp_log.h>

constexpr char* TAG = "GPIO";

void Application::updatePinStates() {
    tt_mutex_lock(mutex, TT_MAX_TICKS);
    // Update pin states
    for (int i = 0; i < pinStates.size(); ++i) {
        pinStates[i] = tt_hal_gpio_get_level(i);
    }
    tt_mutex_unlock(mutex);
}

void Application::updatePinWidgets() {
    tt_lvgl_lock();
    assert(pinStates.size() == pinWidgets.size());
    for (int j = 0; j < pinStates.size(); ++j) {
        int level = pinStates[j];
        lv_obj_t* label = pinWidgets[j];
        void* label_user_data = lv_obj_get_user_data(label);
        // The user data stores the state, so we can avoid unnecessary updates
        if (reinterpret_cast<void*>(level) != label_user_data) {
            lv_obj_set_user_data(label, reinterpret_cast<void*>(level));
            if (level == 0) {
                lv_obj_set_style_text_color(label, lv_color_make(20, 20, 20), LV_STATE_DEFAULT);
            } else {
                lv_obj_set_style_text_color(label, lv_color_make(0, 200, 0), LV_STATE_DEFAULT);
            }
        }
    }
    tt_lvgl_unlock();
}

lv_obj_t* Application::createGpioRowWrapper(lv_obj_t* parent) {
    lv_obj_t* wrapper = lv_obj_create(parent);
    lv_obj_set_style_pad_all(wrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(wrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_size(wrapper, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    return wrapper;
}

// region Task

void Application::onTimer(void* context) {
    Application* app = static_cast<Application*>(context);
    app->updatePinStates();
    app->updatePinWidgets();
}

void Application::startTask() {
    tt_mutex_lock(mutex, TT_MAX_TICKS);
    assert(timer == nullptr);
    timer = tt_timer_alloc(TimerTypePeriodic, onTimer, this);
    tt_timer_start(timer, 100 / portTICK_PERIOD_MS);
    tt_mutex_unlock(mutex);
}

void Application::stopTask() {
    assert(timer);

    tt_timer_stop(timer);
    tt_timer_free(timer);
    timer = nullptr;
}

// endregion Task

static int getSquareSpacing(UiScale scale) {
    if (scale == UiScaleSmallest) {
        return 0;
    } else {
        return 4;
    }
}

void Application::onCreate(AppHandle app) {
    mutex = tt_mutex_alloc(MUTEX_TYPE_RECURSIVE);
}

void Application::onDestroy(AppHandle app) {
    tt_mutex_free(mutex);
}

void Application::onShow(AppHandle app, lv_obj_t* parent) {
    // auto ui_scale = hal::getConfiguration()->uiScale;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(parent, 0, LV_STATE_DEFAULT);

    auto* toolbar = tt_lvgl_toolbar_create_for_app(parent, app);
    lv_obj_align(toolbar, LV_ALIGN_TOP_MID, 0, 0);

    // Main content wrapper, enables scrolling content without scrolling the toolbar
    auto* expansion_wrapper = lv_obj_create(parent);
    lv_obj_set_width(expansion_wrapper, LV_PCT(100));
    lv_obj_set_flex_grow(expansion_wrapper, 1);
    lv_obj_set_style_border_width(expansion_wrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(expansion_wrapper, 0, LV_STATE_DEFAULT);

    auto* centering_wrapper = lv_obj_create(expansion_wrapper);
    lv_obj_set_size(centering_wrapper, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(centering_wrapper, LV_ALIGN_CENTER);
    lv_obj_set_style_border_width(centering_wrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(centering_wrapper, 0, LV_STATE_DEFAULT);

    auto* display = lv_obj_get_display(parent);
    auto horizontal_px = lv_display_get_horizontal_resolution(display);
    auto vertical_px = lv_display_get_vertical_resolution(display);
    bool is_landscape_display = horizontal_px > vertical_px;

    constexpr auto block_width = 16;
    auto ui_scale = tt_hal_configuration_get_ui_scale();
    const auto square_spacing = getSquareSpacing(ui_scale);
    int32_t x_spacing = block_width + square_spacing;
    uint8_t column = 0;
    const uint8_t column_limit = is_landscape_display ? 10 : 5;

    auto* row_wrapper = createGpioRowWrapper(centering_wrapper);
    lv_obj_align(row_wrapper, LV_ALIGN_TOP_MID, 0, 0);

    tt_mutex_lock(mutex, TT_MAX_TICKS);

    auto pin_count = tt_hal_gpio_get_pin_count();
    pinStates.resize(pin_count);
    pinWidgets.resize(pin_count);

    for (int i = 0; i < pin_count; ++i) {
        constexpr uint8_t offset_from_left_label = 4;

        // Add the GPIO number before the first item on a row
        if (column == 0) {
            auto* prefix = lv_label_create(row_wrapper);
            lv_label_set_text_fmt(prefix, "%02d", i);
        }

        // Add a new GPIO status indicator
        auto* status_label = lv_label_create(row_wrapper);
        lv_obj_set_pos(status_label, (column+1) * x_spacing + offset_from_left_label, 0);
        lv_label_set_text_fmt(status_label, "%s", LV_SYMBOL_STOP);
        lv_obj_set_style_text_color(status_label, lv_color_make(20, 20, 20), LV_STATE_DEFAULT);
        pinWidgets[i] = status_label;
        pinStates[i] = false;

        column++;

        if (column >= column_limit) {
            // Add the GPIO number after the last item on a row
            auto* postfix = lv_label_create(row_wrapper);
            lv_label_set_text_fmt(postfix, "%02d", i);
            lv_obj_set_pos(postfix, (column + 1) * x_spacing + offset_from_left_label, 0);

            // Add a new row wrapper underneath the last one
            auto* new_row_wrapper = createGpioRowWrapper(centering_wrapper);
            lv_obj_align_to(new_row_wrapper, row_wrapper, LV_ALIGN_BOTTOM_LEFT, 0, square_spacing);
            row_wrapper = new_row_wrapper;

            column = 0;
        }
    }

    tt_mutex_unlock(mutex);

    startTask();
}

void Application::onHide(AppHandle app) {
    stopTask();

    tt_mutex_lock(mutex, TT_MAX_TICKS);
    pinWidgets.clear();
    pinStates.clear();
    tt_mutex_unlock(mutex);
}
