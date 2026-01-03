#pragma once

#include <TactilityCpp/App.h>

#include <tt_app.h>

#include <Tactility/RecursiveMutex.h>
#include <Tactility/Timer.h>

#include <lvgl.h>
#include <vector>

class Gpio final : public App {

    std::vector<lv_obj_t*> pinWidgets;
    std::vector<bool> pinStates;
    tt::Timer timer = tt::Timer(tt::Timer::Type::Periodic, pdMS_TO_TICKS(100), [this]{
        onTimer();
    });
    tt::RecursiveMutex mutex;

    static lv_obj_t* createGpioRowWrapper(lv_obj_t* parent);
    void onTimer();

public:

    void onShow(AppHandle context, lv_obj_t* parent) override;
    void onHide(AppHandle context) override;

    void startTask();
    void stopTask();

    void updatePinStates();
    void updatePinWidgets();
};
