#pragma once

#include <tt_lock.h>
#include <TactilityCpp/Lock.h>

class Mutex final : public Lock {

    LockHandle handle;

public:

    explicit Mutex(TtMutexType type) : handle(tt_lock_alloc_mutex(type)) {};

    ~Mutex() override {
        tt_lock_free(handle);
    }

    bool lock(TickType timeout = TT_MAX_TICKS) const override {
        return tt_lock_acquire(handle, timeout);
    }

    bool unlock() const override {
        return tt_lock_release(handle);
    }
};


