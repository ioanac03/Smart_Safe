#include "header.h"

void buzzer_init(){
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

void buzzer_beep(int duration_ms) {
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(duration_ms);
    gpio_put(BUZZER_PIN, 0);
}

void buzzer_alarm(int durata_ms) {
    int elapsed = 0;
    while (elapsed < durata_ms) {
        gpio_put(BUZZER_PIN, 1);
        sleep_ms(300);
        gpio_put(BUZZER_PIN, 0);
        sleep_ms(200);
        elapsed += 500;
    }
}