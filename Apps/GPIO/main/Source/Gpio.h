#pragma once

#include <TactilityCpp/App.h>

#include <tt_app.h>
#include <tt_timer.h>

#include <TactilityCpp/Mutex.h>

#include <lvgl.h>
#include <vector>

class Gpio final : public App {

    std::vector<lv_obj_t*> pinWidgets;
    std::vector<bool> pinStates;
    TimerHandle timer;
    Mutex mutex = Mutex(MutexTypeRecursive);

    static lv_obj_t* createGpioRowWrapper(lv_obj_t* parent);
    static void onTimer(void* parameter);

public:

    void onShow(AppHandle context, lv_obj_t* parent) override;
    void onHide(AppHandle context) override;

    void startTask();
    void stopTask();

    void updatePinStates();
    void updatePinWidgets();
};
