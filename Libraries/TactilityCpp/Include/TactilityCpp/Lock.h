#pragma once

#include <tt_lock.h>

class ScopedLock;

/** Represents a lock/mutex */
class Lock {

public:

    virtual ~Lock() = default;

    virtual bool lock(TickType timeout) const = 0;

    bool lock() const { return lock(TT_MAX_TICKS); }

    virtual bool unlock() const = 0;

    ScopedLock asScopedLock() const;
};

/**
 * Represents a lockable instance that is scoped to a specific lifecycle.
 * Once the ScopedLock is destroyed, unlock() is called automatically.
 *
 * In other words:
 * You have to lock() this object manually, but unlock() happens automatically on destruction.
 */
class ScopedLock final : public Lock {

    const Lock& lockable;

public:

    using Lock::lock;

    explicit ScopedLock(const Lock& lockable) : lockable(lockable) {}

    ~ScopedLock() override {
        lockable.unlock(); // We don't care whether it succeeded or not
    }

    bool lock(TickType timeout) const override {
        return lockable.lock(timeout);
    }

    bool unlock() const override {
        return lockable.unlock();
    }
};
