#pragma once

#include "tt_app.h"
#include "tt_thread.h"

#include <Str.h>
#include <lvgl.h>
#include <TactilityCpp/App.h>

class Diceware final : public App {

    AppHandle handle = nullptr;
    lv_obj_t* spinbox = nullptr;
    lv_obj_t* resultLabel = nullptr;
    ThreadHandle jobThread = nullptr;
    uint32_t wordCount = 5;

    static void onClickGenerate(lv_event_t* e);
    static void onSpinboxDecrement(lv_event_t* e);
    static void onSpinboxIncrement(lv_event_t* e);
    static void onHelpClicked(lv_event_t* e);

    static int32_t jobMain(void* data);

    void startJob(uint32_t jobWordCount);
    void onFinishJob(Str result);
    void cleanupJob();

public:

    void onShow(AppHandle context, lv_obj_t* parent) override;
    void onHide(AppHandle context) override;
};
