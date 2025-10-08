#pragma once

#include <tt_preferences.h>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

class Preferences {

    PreferencesHandle handle = nullptr;

public:
    // Open preferences with the given identifier/namespace
    explicit Preferences(const char* identifier) : handle(tt_preferences_alloc(identifier)) {}

    // Non-copyable (owns a handle)
    Preferences(const Preferences&) = delete;
    Preferences& operator=(const Preferences&) = delete;

    // Movable
    Preferences(Preferences&& other) noexcept : handle(other.handle) {
        other.handle = nullptr;
    }

    Preferences& operator=(Preferences&& other) noexcept {
        if (this != &other) {
            if (handle) {
                tt_preferences_free(handle);
            }
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    ~Preferences() {
        if (handle) {
            tt_preferences_free(handle);
        }
    }

    bool optBool(const char* key, bool& out) const {
        return tt_preferences_opt_bool(handle, key, &out);
    }

    bool getBool(const char* key, bool defaultValue = false) const {
        bool value = defaultValue;
        (void) tt_preferences_opt_bool(handle, key, &value);
        return value;
    }

    void putBool(const char* key, bool value) const {
        tt_preferences_put_bool(handle, key, value);
    }

    bool optInt32(const char* key, int32_t& out) const {
        return tt_preferences_opt_int32(handle, key, &out);
    }

    int32_t getInt32(const char* key, int32_t defaultValue = 0) const {
        int32_t value = defaultValue;
        (void) tt_preferences_opt_int32(handle, key, &value);
        return value;
    }

    void putInt32(const char* key, int32_t value) const {
        tt_preferences_put_int32(handle, key, value);
    }

    bool optString(const char* key, char* out, uint32_t outSize) const {
        return tt_preferences_opt_string(handle, key, out, outSize);
    }

    template <size_t N>
    bool optString(const char* key, char (&out)[N]) const {
        return tt_preferences_opt_string(handle, key, out, static_cast<uint32_t>(N));
    }

    std::string getString(const char* key, size_t maxSize, const std::string& defaultValue = std::string()) const {
        if (maxSize == 0) return defaultValue;
        std::string buf;
        buf.resize(maxSize);
        if (tt_preferences_opt_string(handle, key, buf.data(), static_cast<uint32_t>(maxSize))) {
            buf.resize(std::strlen(buf.c_str()));
            return buf;
        }
        return defaultValue;
    }

    void putString(const char* key, const char* value) const {
        tt_preferences_put_string(handle, key, value);
    }
    void putString(const char* key, const std::string& value) const {
        tt_preferences_put_string(handle, key, value.c_str());
    }
};

