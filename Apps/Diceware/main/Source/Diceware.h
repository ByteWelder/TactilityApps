#pragma once

#include "tt_app.h"

#include <Tactility/Thread.h>

#include <string>
#include <lvgl.h>
#include <TactilityCpp/App.h>

#include <memory>

class Diceware final : public App {

    AppHandle handle = nullptr;
    lv_obj_t* spinbox = nullptr;
    lv_obj_t* resultLabel = nullptr;
    std::unique_ptr<tt::Thread> jobThread = nullptr;
    uint32_t wordCount = 5;

    static void onClickGenerate(lv_event_t* e);
    static void onSpinboxDecrement(lv_event_t* e);
    static void onSpinboxIncrement(lv_event_t* e);
    static void onHelpClicked(lv_event_t* e);

    int32_t jobMain();

    void startJob(uint32_t jobWordCount);
    void onFinishJob(std::string result);
    void cleanupJob();

public:

    void onShow(AppHandle context, lv_obj_t* parent) override;
    void onHide(AppHandle context) override;
};
