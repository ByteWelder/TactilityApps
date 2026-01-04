#pragma once

#include <tt_hal_uart.h>
#include <memory>
#include <string>
#include <vector>
#include <freertos/FreeRTOS.h>

class Uart {
    UartHandle handle;

public:

    explicit Uart(UartHandle handle) : handle(handle) {}

    ~Uart() {
        tt_hal_uart_free(handle);
    }

    static std::unique_ptr<Uart> open(size_t index) {
        auto handle = tt_hal_uart_alloc(index);
        return std::make_unique<Uart>(handle);
    }

    static std::vector<std::string> getNames() {
        std::vector<std::string> names;
        size_t count = tt_hal_uart_get_count();
        for (size_t i = 0; i < count; i++) {
            char buffer[64];
            if (tt_hal_uart_get_name(i, buffer, sizeof(buffer))) {
                names.push_back(std::string(buffer));
            }
        }
        return names;
    }

    bool start() const {
        return tt_hal_uart_start(handle);
    }

    bool isStarted() const {
        return tt_hal_uart_is_started(handle);
    }

    bool stop() const {
        return tt_hal_uart_stop(handle);
    }

    size_t readBytes(char* buffer, size_t bufferSize, TickType_t timeout) const {
        return tt_hal_uart_read_bytes(handle, buffer, bufferSize, timeout);
    }

    bool readByte(char* output, TickType_t timeout) const {
        return tt_hal_uart_read_bytes(handle, output, 1, timeout);
    }

    size_t writeBytes(const char* buffer, size_t bufferSize, TickType_t timeout) const {
        return tt_hal_uart_write_bytes(handle, buffer, bufferSize, timeout);
    }

    size_t available() const {
        return tt_hal_uart_available(handle);
    }

    bool setBaudRate(size_t baud_rate) const {
        return tt_hal_uart_set_baud_rate(handle, baud_rate);
    }

    uint32_t getBaudRate() const {
        return tt_hal_uart_get_baud_rate(handle);
    }

    void flushInput() const {
        tt_hal_uart_flush_input(handle);
    }
};
