#pragma once
#include <hardware/gpio.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <pico/types.h>
#include <cstdint>

namespace pico_push {
    using pin_t = uint;

    enum class PushState : uint8_t {
        unpressed,
        pushed,
        depressed,
        released
    };

    constexpr bool PUSH_PULL_UP = true;

    constexpr bool PUSH_PULL_DOWN = false;

    class Push {

    public:
        Push(const pin_t& pin, const bool& pull_up_button = PUSH_PULL_DOWN, const absolute_time_t& debounce = 10e3);

        void initialize();

        bool now() const;

        PushState state() const;

        absolute_time_t pushed_at() const;

        absolute_time_t released_at() const;

        uint64_t held_for() const;

        PushState update();

    private:
        const pin_t pin;

        const bool pull_up_button;

        const absolute_time_t& debounce;

        PushState last_state;

        absolute_time_t last_read;

        absolute_time_t _pushed_at;

        absolute_time_t _released_at;
    };
}
