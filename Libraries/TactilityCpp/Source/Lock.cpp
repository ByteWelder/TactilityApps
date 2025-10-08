#include <TactilityCpp/Lock.h>

ScopedLock Lock::asScopedLock() const {
    return ScopedLock(*this);
}

