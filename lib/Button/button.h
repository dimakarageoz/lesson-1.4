#ifndef BUTTON_LIB
#define BUTTON_LIB

#include <driver/gpio.h>

typedef struct {
    int64_t *now;
    int buttonState;
    int prevButtonState;
    long changeTime;
    int8_t delayStart;
} DelayButtonState;

void delayButtonHandler(gpio_num_t button_pin, int32_t delay, DelayButtonState *state, void (*handler)(int));

void createDelayButtonState(DelayButtonState* state, int64_t *now);

#endif