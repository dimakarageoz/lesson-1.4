#include <driver/gpio.h>

typedef struct {
    uint8_t ledBlinkSpeed;
    int64_t lastBlink;
    gpio_num_t activeLed;
    int64_t now;
} LoopContext;