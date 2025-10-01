#pragma once

#include "tt_app.h"

#include <lvgl.h>
#include <deque>
#include <Str.h>

class Calculator {

    lv_obj_t* displayLabel;
    lv_obj_t* resultLabel;
    char formulaBuffer[128] = {0}; // Stores the full input expression
    bool newInput = true;

    static void button_event_cb(lv_event_t* e);
    void handleInput(const char* txt);
    void evaluateExpression();
    double computeFormula();
    static std::deque<Str> infixToRPN(const Str& infix);
    static double evaluateRPN(std::deque<Str> rpnQueue);
    void resetCalculator();

public:

    void onShow(AppHandle context, lv_obj_t* parent);
};