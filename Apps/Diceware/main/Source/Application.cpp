#include "Application.h"

#include <tt_file.h>
#include <tt_lock.h>
#include <tt_lvgl.h>
#include <tt_lvgl_toolbar.h>

#include <esp_random.h>
#include <esp_log.h>

constexpr char* TAG = "Diceware";

static void skipNewlines(FILE* file, const int count) {
    char c;
    int count_in_file = 0;
    while (count_in_file < count && fread(&c, 1, 1, file)) {
        if (c == '\n') {
            count_in_file++;
        }
    }
}

static Str readWord(FILE* file) {
    char c;
    Str result;
    // Read word until newline
    while (fread(&c, 1, 1, file) && c != '\n') { result.append(c); }
    return result;
}

static Str readWordAtLine(const AppHandle handle, const int lineIndex) {
    char path[256];
    size_t size = 256;
    tt_app_get_assets_child_path(handle, "eff_large_wordlist.txt", path, &size);
    if (size == 0) {
        ESP_LOGE(TAG, "Failed to get assets path");
        return "";
    }

    auto lock = tt_lock_alloc_for_file(path);
    Str word;
    if (tt_lock_acquire(lock, TT_MAX_TICKS)) {
        FILE* file = fopen(path, "r");
        if (file != nullptr) {
            skipNewlines(file, lineIndex);
            word = readWord(file);
            fclose(file);
        } else { ESP_LOGE(TAG, "Failed to open %s", path); }
        tt_lock_release(lock);
    } else { ESP_LOGE(TAG, "Failed to acquire lock for %s", path); }
    tt_lock_free(lock);
    return word;
}

int32_t Application::jobMain(void* data) {
    Application* application = static_cast<Application*>(data);
    Str result;
    for (int i = 0; i < application->wordCount; i++) {
        constexpr int line_count = 7776;
        const auto line_index = esp_random() % line_count;
        auto word = readWordAtLine(application->handle, line_index);
        result.appendf("%s ", word.c_str());
    }

    application->onFinishJob(result);

    return 0;
}

void Application::cleanupJob() {
    if (jobThread != nullptr) {
        tt_thread_join(jobThread, TT_MAX_TICKS);
        tt_thread_free(jobThread);
    }
}

void Application::startJob(uint32_t jobWordCount) {
    cleanupJob();

    wordCount = jobWordCount;
    jobThread = tt_thread_alloc_ext("Diceware", 4096, jobMain, this);
    tt_thread_start(jobThread);
}

void Application::onFinishJob(Str result) {
    tt_lvgl_lock();
    lv_label_set_text(resultLabel, result.c_str());
    tt_lvgl_unlock();
}

void Application::onClickGenerate(lv_event_t* e) {
    auto* application = static_cast<Application*>(lv_event_get_user_data(e));
    auto* spinbox = application->spinbox;

    lv_label_set_text(application->resultLabel, "Generating...");

    const auto word_count = lv_spinbox_get_value(spinbox);
    application->startJob(word_count);
}

void Application::onSpinboxDecrement(lv_event_t* e) {
    auto* spinbox = static_cast<lv_obj_t*>(lv_event_get_user_data(e));
    lv_spinbox_decrement(spinbox);
}

void Application::onSpinboxIncrement(lv_event_t* e) {
    auto* spinbox = static_cast<lv_obj_t*>(lv_event_get_user_data(e));
    lv_spinbox_increment(spinbox);
}

void Application::onShow(AppHandle appHandle, lv_obj_t* parent) {
    handle = appHandle;

    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(parent, 0, LV_STATE_DEFAULT);

    tt_lvgl_toolbar_create_for_app(parent, appHandle);

    auto* wrapper = lv_obj_create(parent);
    lv_obj_set_style_border_width(wrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(wrapper, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(wrapper, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_width(wrapper, LV_PCT(100));
    lv_obj_set_flex_grow(wrapper, 1);

    auto* top_row_wrapper = lv_obj_create(wrapper);
    lv_obj_set_style_border_width(top_row_wrapper, 0, LV_STATE_DEFAULT);
    lv_obj_set_size(top_row_wrapper, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(top_row_wrapper, 0, LV_STATE_DEFAULT);

    auto* generate_button = lv_button_create(top_row_wrapper);
    lv_obj_align(generate_button, LV_ALIGN_LEFT_MID, 0, 0);
    auto* generate_label = lv_label_create(generate_button);
    lv_label_set_text(generate_label, "Generate");
    lv_obj_add_event_cb(generate_button, onClickGenerate, LV_EVENT_SHORT_CLICKED, this);

    spinbox = lv_spinbox_create(top_row_wrapper);
    lv_spinbox_set_range(spinbox, 4, 12);
    lv_spinbox_set_value(spinbox, 5);
    lv_spinbox_set_step(spinbox, 1);
    lv_spinbox_set_digit_format(spinbox, 1, 0);
    lv_obj_set_width(spinbox, 30);

    auto* spinbox_dec_button = lv_button_create(top_row_wrapper);
    lv_obj_set_style_bg_image_src(spinbox_dec_button, LV_SYMBOL_MINUS, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(spinbox_dec_button, onSpinboxDecrement, LV_EVENT_SHORT_CLICKED, spinbox);
    lv_obj_set_style_pad_all(spinbox_dec_button, 16, LV_STATE_DEFAULT);

    auto* spinbox_inc_button = lv_button_create(top_row_wrapper);
    // lv_obj_align_to(spinbox_inc_button, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_image_src(spinbox_inc_button, LV_SYMBOL_PLUS, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(spinbox_inc_button, onSpinboxIncrement, LV_EVENT_SHORT_CLICKED, spinbox);
    lv_obj_set_style_pad_all(spinbox_inc_button, 16, LV_STATE_DEFAULT);

    // Align spinbox widgets
    lv_obj_align(spinbox_inc_button, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_align_to(spinbox, spinbox_inc_button, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_align_to(spinbox_dec_button, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);

    auto* result_wrapper = lv_obj_create(wrapper);
    lv_obj_set_flex_grow(result_wrapper, 1);
    lv_obj_set_width(result_wrapper, LV_PCT(100));
    lv_obj_set_style_pad_all(result_wrapper, 0, LV_STATE_DEFAULT);

    resultLabel = lv_label_create(result_wrapper);
    // See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/random.html
    lv_label_set_text(resultLabel, "Press Generate button\nEnable Wi-Fi for high entropy.\nNo need to connect.");
    lv_label_set_long_mode(resultLabel, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_size(resultLabel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_align(resultLabel, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(resultLabel, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);
}

void Application::onHide(AppHandle context) { cleanupJob(); }