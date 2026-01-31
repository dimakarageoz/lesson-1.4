#include <stdlib.h>

#include "Button.h"

void createDelayButtonState(
    DelayButtonState *state,
    int64_t *now 
) {
    state->now = now;
    state->buttonState = 0;
    state->prevButtonState = 0;
    state->changeTime = 0;
    state->delayStart = 0;
}

void delayButtonHandler(
    gpio_num_t button_pin, 
    int32_t delay,
    DelayButtonState *state,
    void (*handler)(int)
) {
    state->buttonState = gpio_get_level(button_pin);

    int64_t timeNow = *(state->now);
     
    if (state->buttonState != state->prevButtonState) {
        state->prevButtonState = state->buttonState;

        state->changeTime = timeNow;
        state->delayStart = 1;
    }

    if (state->delayStart && llabs(timeNow - state->changeTime) > delay) {
        state->delayStart = 0;

        handler(state->buttonState);
    }
}

