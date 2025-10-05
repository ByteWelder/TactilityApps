#pragma once

#include "tt_app.h"
#include "tt_mutex.h"
#include "tt_timer.h"

#include <lvgl.h>
#include <vector>

class Application final {

    std::vector<lv_obj_t*> pinWidgets;
    std::vector<bool> pinStates;
    TimerHandle timer;
    MutexHandle mutex;

    static lv_obj_t* createGpioRowWrapper(lv_obj_t* parent);
    static void onTimer(void* parameter);

public:

    void onCreate(AppHandle app);
    void onDestroy(AppHandle app);
    void onShow(AppHandle context, lv_obj_t* parent);
    void onHide(AppHandle context);

    void startTask();
    void stopTask();

    void updatePinStates();
    void updatePinWidgets();
};
