#pragma once

#include <tt_thread.h>

class Thread {
    ThreadHandle handle;
public:

    Thread() : handle(tt_thread_alloc()) {}

    Thread(
        const char* name,
        uint32_t stackSize,
        ThreadCallback callback,
        void* _Nullable callbackContext
    ) : handle(tt_thread_alloc_ext(name, stackSize, callback, callbackContext)) {}

    ~Thread() {
        tt_thread_free(handle);
    }

    void start() const {
        tt_thread_start(handle);
    }

    bool join(TickType timeout = TT_MAX_TICKS) const {
        return tt_thread_join(handle, timeout);
    }

    void setName(const char* name) const {
        tt_thread_set_name(handle, name);
    }

    void setStackSize(size_t stackSize) const {
        tt_thread_set_stack_size(handle, stackSize);
    }

    void setAffinity(int affinity) const {
        tt_thread_set_affinity(handle, affinity);
    }

    void setCallback(ThreadCallback callback, void* _Nullable callbackContext) const {
        tt_thread_set_callback(handle, callback, callbackContext);
    }

    void setPriority(ThreadPriority priority) const {
        tt_thread_set_priority(handle, priority);
    }

    void setStateCallback(ThreadStateCallback callback, void* _Nullable callbackContext) const {
        tt_thread_set_state_callback(handle, callback, callbackContext);
    }

    ThreadState getState() const {
        return tt_thread_get_state(handle);
    }

    ThreadId getId() const {
        return tt_thread_get_id(handle);
    }

    int32_t getReturnCode() const {
        return tt_thread_get_return_code(handle);
    }
};
