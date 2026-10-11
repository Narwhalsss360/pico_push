#include <pico_push.hpp>

using namespace pico_push;

static inline int64_t absolute_time_diff_ms(absolute_time_t from, absolute_time_t to) {
    return absolute_time_diff_us(from, to) / 1000;
}

Push::Push(const pin_t& pin, const bool& pull_up_button, const uint32_t& debounce)
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
    if (last_state != PushState::released) {
        return _pushed_at;
    }

    return nil_time;
}

absolute_time_t Push::released_at() const {
    if (last_state != PushState::pushed) {
        return _released_at;
    }

    return nil_time;
}

int64_t Push::held_for() const {
    if (last_state == PushState::unpressed || last_state == PushState::released) {
        return absolute_time_diff_ms(_released_at, _pushed_at);
    }

    if (last_state == PushState::depressed) {
        return absolute_time_diff_ms(get_absolute_time(), _pushed_at);
    }

    return 0;
}

PushState Push::update() {
    if (absolute_time_diff_ms(last_read, get_absolute_time()) < static_cast<int64_t>(debounce)) {
        if (last_state == PushState::pushed) {
            last_state = PushState::depressed;
        } else if (last_state == PushState::released) {
            last_state = PushState::unpressed;
        }
        return last_state;
    }
    last_read = get_absolute_time();

    switch (last_state) {
        case PushState::unpressed: {
            last_state = now() ? PushState::pushed : PushState::unpressed;
            break;
        }
        case PushState::pushed: {
            last_state = now() ? PushState::depressed : PushState::released;
            _pushed_at = get_absolute_time();
            break;
        }
        case PushState::depressed: {
            last_state = now() ? PushState::depressed : PushState::released;
            break;
        }
        case PushState::released: {
            last_state = now() ? PushState::pushed : PushState::unpressed;
            _released_at = get_absolute_time();
            break;
        }
        default: {
            last_state = PushState::unpressed;
        }
    }

    return last_state;
}
