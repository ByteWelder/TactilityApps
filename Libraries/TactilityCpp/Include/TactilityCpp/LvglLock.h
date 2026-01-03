#pragma once

#include <Tactility/Lock.h>
#include <tt_lvgl.h>

class LvglLock final : public tt::Lock {

public:

    bool lock(TickType_t timeout = tt::kernel::MAX_TICKS) const override {
        return tt_lvgl_lock(timeout);
    }


    void unlock() const override {
        tt_lvgl_unlock();
    }
};


