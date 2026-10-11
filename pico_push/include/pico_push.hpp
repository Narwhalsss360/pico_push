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

    constexpr bool DEFAULT_PULL = PUSH_PULL_DOWN;

    constexpr uint64_t DEFAULT_DEBOUNCE = 10;

    class Push {

    public:
        Push(const pin_t& pin, const bool& pull_up_button = DEFAULT_PULL, const uint32_t& debounce = DEFAULT_DEBOUNCE);

        void initialize();

        bool now() const;

        PushState state() const;

        absolute_time_t pushed_at() const;

        absolute_time_t released_at() const;

        int64_t held_for() const;

        PushState update();

        const pin_t pin;

        const bool pull_up_button;

        const uint32_t debounce;

    private:
        PushState last_state;

        absolute_time_t last_read;

        absolute_time_t _pushed_at;

        absolute_time_t _released_at;
    };
}
