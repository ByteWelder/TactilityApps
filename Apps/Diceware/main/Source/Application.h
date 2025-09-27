#pragma once

#include "tt_app.h"

#include <lvgl.h>

class Application final {

    AppHandle handle = nullptr;
    lv_obj_t* spinbox = nullptr;
    lv_obj_t* resultLabel = nullptr;

    static void onClickGenerate(lv_event_t* e);
    static void onSpinboxDecrement(lv_event_t* e);
    static void onSpinboxIncrement(lv_event_t* e);

public:

    void onShow(AppHandle context, lv_obj_t* parent);

};
