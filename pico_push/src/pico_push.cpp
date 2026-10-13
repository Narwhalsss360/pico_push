#include <pico_push.hpp>

using namespace pico_push;

static inline int64_t absolute_time_diff_ms(absolute_time_t from, absolute_time_t to) {
    return absolute_time_diff_us(from, to) / 1000;
}

Push::Push(const pin_t pin, const bool pull_up_button, const uint32_t debounce)
    :
        pin(pin),
        pull_up_button(pull_up_button),
        debounce(debounce),
        last_read(nil_time),
        _pushed_at(nil_time),
        _released_at(nil_time) {}

void Push::initialize() {
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_pulls(pin, pull_up_button, !pull_up_button);

    last_read = get_absolute_time();
    if (now()) {
        _pushed_at = get_absolute_time();
        last_state = PushState::depressed;
    } else {
        _released_at = get_absolute_time();
        last_state = PushState::unpressed;
    }
}

bool Push::now() const {
    return pull_up_button ? !gpio_get(pin) : gpio_get(pin);
}

PushState Push::state() const {
    return last_state;
}

absolute_time_t Push::pushed_at() const {
    return _pushed_at;
}

absolute_time_t Push::released_at() const {
    return _released_at;
}

int64_t Push::held_for() const {
    if (last_state == PushState::pushed || last_state == PushState::depressed) {
        return absolute_time_diff_ms(_pushed_at, get_absolute_time());
    }
    return absolute_time_diff_ms(_pushed_at, _released_at);
}

PushState Push::update() {
    if (absolute_time_diff_ms(last_read, get_absolute_time()) < static_cast<int64_t>(debounce)) {
        return last_state;
    }

    const bool is_pressed = now();
    switch (last_state) {
        case PushState::unpressed: {
            if (is_pressed) {
                last_state = PushState::pushed;
                _pushed_at = get_absolute_time();
                last_read = get_absolute_time();
            }
            break;
        }
        case PushState::pushed: {
            last_state = is_pressed ? PushState::depressed : PushState::released;
            if (!is_pressed) {
                _released_at = get_absolute_time();
                last_read = get_absolute_time();
            }
            break;
        }
        case PushState::depressed: {
            if (!is_pressed) {
                last_state = PushState::released;
                _released_at = get_absolute_time();
                last_read = get_absolute_time();
            }
            break;
        }
        case PushState::released: {
            last_state = is_pressed ? PushState::pushed : PushState::unpressed;
            if (is_pressed) {
                _pushed_at = get_absolute_time();
                last_read = get_absolute_time();
            }
            break;
        }
        default: {
            last_state = PushState::unpressed;
            break;
        }
    }

    return last_state;
}
