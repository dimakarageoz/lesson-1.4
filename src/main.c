#include <esp_timer.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Libs
#include "Button.h"

// Includes
#include "env-var.h"
#include "logs.h"
#include "pins-configuration.h"

typedef struct {
    uint8_t ledBlinkSpeed;
    int64_t lastBlink;
    gpio_num_t activeLed;
    int64_t now;
} LoopContext;

DelayButtonState buttonConfigOne;
DelayButtonState buttonConfigTwo;
LoopContext context;

void gpioButtonsSetup() {
    gpio_config_t gpio_leds_config = {
        .pin_bit_mask = (1 << PIN_BUTTON_1) | (1 << PIN_BUTTON_2),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_leds_config);

    createDelayButtonState(&buttonConfigOne, &context.now);
    createDelayButtonState(&buttonConfigTwo, &context.now);
}

void gpioLedsSetup() {
    gpio_config_t gpio_leds_config = {
        .pin_bit_mask = (1 << PIN_LED_COL1) | (1 << PIN_LED_COL2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_leds_config);

    gpio_set_level(PIN_LED_COL1, 0);
    gpio_set_level(PIN_LED_COL2, 0);
}

void setupContext() {
    context.ledBlinkSpeed = 0;
    context.lastBlink = esp_timer_get_time() / 1000;
    context.activeLed = PIN_LED_COL1;
}

void butonSpeedDownHandler(int buttonState) {
    if (!buttonState) {
        if (context.ledBlinkSpeed > 0) {
            context.ledBlinkSpeed--;
        }

        writeLog("Speed DOWN %d", context.ledBlinkSpeed);
    }
}

void butonSpeedUpHandler(int buttonState) {
    if (!buttonState) {
        context.ledBlinkSpeed++;
        
        if (context.ledBlinkSpeed > BLINK_SPEED_STEPS) {
            context.ledBlinkSpeed = BLINK_SPEED_STEPS;
        }
      
        writeLog("Speed UP %d", context.ledBlinkSpeed);
    }
}

void ledBlink() {
    int64_t passedTimeAfterBlink = context.now - context.lastBlink;
    uint32_t blinkDuration = MAX_BLINK_DURATION - MAX_BLINK_DURATION / BLINK_SPEED_STEPS * context.ledBlinkSpeed;

    if (passedTimeAfterBlink < blinkDuration) {
        return;
    }

    switch (context.activeLed) {
        case PIN_LED_COL1:
            gpio_set_level(PIN_LED_COL1, 0);
            gpio_set_level(PIN_LED_COL2, 1);

            context.activeLed = PIN_LED_COL2;
            break;

        case PIN_LED_COL2:
            gpio_set_level(PIN_LED_COL2, 0);
            gpio_set_level(PIN_LED_COL1, 1);

            context.activeLed = PIN_LED_COL1;
            break;
        
        default:
            break;
    }

    context.lastBlink = context.now;
}

void timeCounter() {
    context.now = esp_timer_get_time() / 1000;
}

void setup() {
    gpioLedsSetup();
    gpioButtonsSetup();
    setupContext();
}

void loopHandler() {
    timeCounter();
    
    delayButtonHandler(PIN_BUTTON_1, BUTTON_CLICK_DELAY, &buttonConfigOne, butonSpeedDownHandler);    
    delayButtonHandler(PIN_BUTTON_2, BUTTON_CLICK_DELAY, &buttonConfigTwo, butonSpeedUpHandler);    

    ledBlink();

    // To remove issue from Watchdog. I have to make this delay to process Watchdog subtasks
    vTaskDelay(pdMS_TO_TICKS(10));
}

void app_main() {
    setup();

    while (1) {
        loopHandler();
    }   
}