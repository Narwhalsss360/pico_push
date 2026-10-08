#include <pico_push.hpp>

using namespace pico_push;

Push::Push(const pin_t& pin, const bool& pull_up_button, const absolute_time_t& debounce)
    :
        pin(pin),
        pull_up_button(pull_up_button),
        debounce(debounce),
        last_read(0),
        pushed_at(0),
        released_at(0) {}

void Push::initialize() {
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_pulls(pin, pull_up_button, !pull_up_button);

    last_read = get_absolute_time();
    if (now()) {
        pushed_at = get_absolute_time();
        last_state = PushState::depressed;
    } else {
        released_at = get_absolute_time();
        last_state = PushState::unpressed;
    }
}

bool Push::now() const {
    return pull_up_button ? !gpio_get(pin) : gpio_get(pin);
}

PushState Push::state() const {
    return last_state;
}

absolute_time_t Push::held_for() const {
    if (last_state == PushState::unpressed || last_state == PushState::released) {
        return released_at - pushed_at;
    }

    if (last_state == PushState::depressed) {
        return get_absolute_time() - pushed_at;
    }

    return 0;
}

PushState Push::update() {
    if (get_absolute_time() - last_read >= debounce) {
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
            pushed_at = get_absolute_time();
            break;
        }
        case PushState::depressed: {
            last_state = now() ? PushState::depressed : PushState::released;
            break;
        }
        case PushState::released: {
            last_state = now() ? PushState::pushed : PushState::unpressed;
            released_at = get_absolute_time();
            break;
        }
        default: {
            last_state = PushState::unpressed;
        }
    }

    return last_state;
}
