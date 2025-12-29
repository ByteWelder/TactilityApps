#pragma once

#include <tt_app.h>

#include <lvgl.h>
#include <TactilityCpp/App.h>

#include "TwoElevenUi.h"
#include "TwoElevenLogic.h"
#include "TwoElevenHelpers.h"

class TwoEleven final : public App {

    static void twoElevenEventCb(lv_event_t* e);
    static void newGameBtnEvent(lv_event_t* e);
    static void create_game(lv_obj_t* parent, uint16_t size, lv_obj_t* toolbar);
    static void create_selection(lv_obj_t* parent, lv_obj_t* toolbar);
    static void size_select_cb(lv_event_t* e);

public:

    void onShow(AppHandle context, lv_obj_t* parent) override;
    void onResult(AppHandle appHandle, void* _Nullable data, AppLaunchId launchId, AppResult result, BundleHandle resultData) override;
};