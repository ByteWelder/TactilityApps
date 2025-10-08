#pragma once

#include <TactilityCpp/Lock.h>
#include <tt_lvgl.h>

class LvglLock final : public Lock {

public:

    bool lock(TickType timeout = TT_MAX_TICKS) const override {
        tt_lvgl_lock(timeout);
        return true;
    }


    bool unlock() const override {
        tt_lvgl_unlock();
        return true;
    }
};


