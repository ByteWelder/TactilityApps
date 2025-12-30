#include "TwoEleven.h"

#include <tt_lvgl_toolbar.h>
#include <tt_app_alertdialog.h>
#include <TactilityCpp/LvglLock.h>

constexpr auto* TAG = "TwoEleven";

static lv_obj_t* scoreLabel = nullptr;
static lv_obj_t* scoreWrapper = nullptr;
static lv_obj_t* toolbar = nullptr;
static lv_obj_t* mainWrapper = nullptr;
static lv_obj_t* newGameWrapper = nullptr;
static lv_obj_t* gameObject = nullptr;

static uint16_t selectedSize = 4;

void TwoEleven::twoElevenEventCb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj_2048 = lv_event_get_target_obj(e);
    lv_obj_t* scoreLabel = (lv_obj_t *)lv_event_get_user_data(e);

    const char* alertDialogLabels[] = { "OK" };

    if (code == LV_EVENT_VALUE_CHANGED) {
        if (twoeleven_get_best_tile(obj_2048) >= 2048) {
            char message[64];
            sprintf(message, "YOU WIN!\n\nSCORE: %d", twoeleven_get_score(obj_2048));
            tt_app_alertdialog_start("YOU WIN!", message, alertDialogLabels, 1);
        } else if (twoeleven_get_status(obj_2048)) {
            char message[64];
            sprintf(message, "GAME OVER!\n\nSCORE: %d", twoeleven_get_score(obj_2048));
            tt_app_alertdialog_start("GAME OVER!", message, alertDialogLabels, 1);
        } else {
            lv_label_set_text_fmt(scoreLabel, "SCORE: %d", twoeleven_get_score(obj_2048));
        }
    }
}

void TwoEleven::newGameBtnEvent(lv_event_t* e) {
    lv_obj_t* obj_2048 = (lv_obj_t *)lv_event_get_user_data(e);
    twoeleven_set_new_game(obj_2048);
}

void TwoEleven::create_game(lv_obj_t* parent, uint16_t size, lv_obj_t* toolbar) {
    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    //game...
    gameObject = twoeleven_create(parent, size);
    lv_obj_set_style_text_font(gameObject, lv_font_get_default(), 0);
    lv_obj_set_size(gameObject, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_grow(gameObject, 1);

    scoreWrapper = lv_obj_create(toolbar);
    lv_obj_set_size(scoreWrapper, LV_SIZE_CONTENT, LV_PCT(100));
    lv_obj_set_style_pad_top(scoreWrapper, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(scoreWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(scoreWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(scoreWrapper, 10, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(scoreWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(scoreWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(scoreWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(scoreWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_remove_flag(scoreWrapper, LV_OBJ_FLAG_SCROLLABLE);

    //toolbar new score
    scoreLabel = lv_label_create(scoreWrapper);
    lv_label_set_text_fmt(scoreLabel, "SCORE: %d", twoeleven_get_score(gameObject));
    lv_obj_set_style_text_align(scoreLabel, LV_TEXT_ALIGN_LEFT, LV_STATE_DEFAULT);
    lv_obj_align(scoreLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(scoreLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_text_font(scoreLabel, lv_font_get_default(), 0);
    lv_obj_set_style_text_color(scoreLabel, lv_palette_main(LV_PALETTE_AMBER), LV_PART_MAIN);
    lv_obj_add_event_cb(gameObject, twoElevenEventCb, LV_EVENT_ALL, scoreLabel);

    newGameWrapper = lv_obj_create(toolbar);
    lv_obj_set_width(newGameWrapper, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(newGameWrapper, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(newGameWrapper, 2, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(newGameWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(newGameWrapper, 0, LV_STATE_DEFAULT);

    //toolbar reset
    lv_obj_t* newGameBtn = lv_btn_create(newGameWrapper);
    lv_obj_set_size(newGameBtn, 34, 34);
    lv_obj_set_style_pad_all(newGameBtn, 0, LV_STATE_DEFAULT);
    lv_obj_align(newGameBtn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(newGameBtn, newGameBtnEvent, LV_EVENT_CLICKED, gameObject);

    lv_obj_t* btnLabel = lv_image_create(newGameBtn);
    lv_image_set_src(btnLabel, LV_SYMBOL_REFRESH);
    lv_obj_align(btnLabel, LV_ALIGN_CENTER, 0, 0);
}

void TwoEleven::create_selection(lv_obj_t* parent, lv_obj_t* toolbar) {
    lv_obj_t* selection = lv_obj_create(parent);
    lv_obj_set_size(selection, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(selection, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(selection, 1);
    lv_obj_remove_flag(selection, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(selection, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(selection, 0, LV_STATE_DEFAULT);

    lv_obj_t* titleWrapper = lv_obj_create(selection);
    lv_obj_set_size(titleWrapper, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(titleWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(titleWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(titleWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(titleWrapper, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(titleWrapper, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(titleWrapper, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* titleLabel = lv_label_create(titleWrapper);
    lv_label_set_text(titleLabel, "Select Matrix Size");
    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(titleLabel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_t* controlsWrapper = lv_obj_create(titleWrapper);
    lv_obj_set_size(controlsWrapper, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(controlsWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(controlsWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(controlsWrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(controlsWrapper, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(controlsWrapper, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(controlsWrapper, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* touchControlsLabel = lv_label_create(controlsWrapper);
    lv_label_set_text(touchControlsLabel, "Touchscreen:\nSwipe up, down, left, right to move tiles.");
    lv_obj_set_style_text_font(touchControlsLabel, lv_font_get_default(), 0);
    lv_obj_set_style_text_align(touchControlsLabel, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t* keyControlsLabel = lv_label_create(controlsWrapper);
    lv_label_set_text_fmt(keyControlsLabel, "Keyboard:\nUse arrow keys (%s, %s, %s, %s) to move tiles.", LV_SYMBOL_UP, LV_SYMBOL_DOWN, LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_font(keyControlsLabel, lv_font_get_default(), 0);
    lv_obj_set_style_text_align(keyControlsLabel, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t* buttonContainer = lv_obj_create(selection);
    lv_obj_set_flex_flow(buttonContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(buttonContainer, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(buttonContainer, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(buttonContainer, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(buttonContainer, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(buttonContainer, 0, LV_STATE_DEFAULT);

    for(int s = 3; s <= 6; s++) {
        lv_obj_t* btn = lv_btn_create(buttonContainer);
        lv_obj_set_size(btn, 60, 40);
        lv_obj_t* lbl = lv_label_create(btn);
        char txt[10];
        sprintf(txt, "%dx%d", s, s);
        lv_label_set_text(lbl, txt);
        lv_obj_center(lbl);
        lv_obj_add_event_cb(btn, size_select_cb, LV_EVENT_CLICKED, (void*)s);
    }
}

void TwoEleven::size_select_cb(lv_event_t* e) {
    selectedSize = (uint16_t)(uintptr_t)lv_event_get_user_data(e);
    lv_obj_t* selection = lv_obj_get_parent(lv_event_get_target_obj(e));
    lv_obj_t* selectionWrapper = lv_obj_get_parent(selection);
    lv_obj_clean(selectionWrapper);
    scoreLabel = nullptr;
    scoreWrapper = nullptr;
    newGameWrapper = nullptr;
    gameObject = nullptr;
    create_game(selectionWrapper, selectedSize, toolbar);
}

void TwoEleven::onShow(AppHandle appHandle, lv_obj_t* parent) {
    lv_obj_remove_flag(parent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    toolbar = tt_lvgl_toolbar_create_for_app(parent, appHandle);
    lv_obj_align(toolbar, LV_ALIGN_TOP_MID, 0, 0);

    mainWrapper = lv_obj_create(parent);
    lv_obj_set_width(mainWrapper, LV_PCT(100));
    lv_obj_set_height(mainWrapper, LV_PCT(100));
    lv_obj_set_flex_grow(mainWrapper, 1);
    lv_obj_set_style_pad_all(mainWrapper, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_row(mainWrapper, 2, LV_PART_MAIN);
    lv_obj_set_style_pad_column(mainWrapper, 2, LV_PART_MAIN);
    lv_obj_set_style_border_width(mainWrapper, 0, LV_PART_MAIN);
    lv_obj_remove_flag(mainWrapper, LV_OBJ_FLAG_SCROLLABLE);

    create_selection(mainWrapper, toolbar);
}

void TwoEleven::onResult(AppHandle appHandle, void* _Nullable data, AppLaunchId launchId, AppResult result, BundleHandle resultData) {
    if (result == APP_RESULT_OK && resultData != nullptr) {
        // Dialog closed with OK, go back to selection
        tt_lvgl_lock(TT_LVGL_DEFAULT_LOCK_TIME);
        lv_obj_clean(mainWrapper);
        scoreLabel = nullptr;
        scoreWrapper = nullptr;
        newGameWrapper = nullptr;
        gameObject = nullptr;
        create_selection(mainWrapper, toolbar);
        tt_lvgl_unlock();
    }
}
