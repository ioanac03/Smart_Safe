#include "header.h"
const char PAROLA[] = "1234";

// ===== LDR =====
void ldr_init() {
    adc_init();
    adc_gpio_init(LDR_PIN);
}

uint16_t ldr_read() {
    adc_select_input(LDR_ADC_CH);
    return adc_read(); // 0-4095
}

bool ldr_lumina_detectata() {
    printf("LDR ADC: %d\n", ldr_read());
    return ldr_read() > LDR_THRESHOLD;
}

// ===== SERVO =====
void servo_init() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_set_clkdiv(slice, 64.0f);
    pwm_set_wrap(slice, 39062);
    pwm_set_enabled(slice, true);
}

void servo_set_angle(int angle) {
    uint slice   = pwm_gpio_to_slice_num(SERVO_PIN);
    uint channel = pwm_gpio_to_channel(SERVO_PIN);
    uint pulse = 1953 + (angle * 1953) / 180;
    pwm_set_chan_level(slice, channel, pulse);
}

void servo_deschide() { servo_set_angle(90); }
void servo_inchide()  { servo_set_angle(0);  }


// ===== LOCKOUT - pauza 30s cu countdown pe LCD =====
void lockout_sequence() {
    printf("BLOCAT 30 secunde!\n");
    for (int i = LOCKOUT_SEC; i > 0; i--) {
        lcd_show_lockout(i);
        // Buzzer suna in pauza
        gpio_put(BUZZER_PIN, 1); sleep_ms(100);
        gpio_put(BUZZER_PIN, 0); sleep_ms(900);
        printf("Lockout: %d s ramase\n", i);
    }
    lcd_show_pin(0);
}

// ===== MAIN =====
int main() {
    stdio_init_all();
    keypad_init();
    servo_init();
    lcd_init();
    buzzer_init();
    ldr_init();

    servo_inchide();
    lcd_show_pin(0);

    char input[PIN_LENGTH + 1] = {0};
    int idx           = 0;
    int incercari     = 0;
    bool seif_deschis = false;
    bool alarma_ldr   = false;

    printf("Seif pornit.\n");

    while (true) {

        // ===== VERIFICARE LDR =====
        // Alarma daca detecteaza lumina si seiful e inchis
        if (!seif_deschis && ldr_lumina_detectata()) {
            if (!alarma_ldr) {
                alarma_ldr = true;
                printf("ALARMA: lumina detectata in seif inchis!\n");
                lcd_show_alarm();
            }
            buzzer_beep(200);
            sleep_ms(300);
            continue; // Nu accepta input keypad cat timp e alarma
        } else if (alarma_ldr && !ldr_lumina_detectata()) {
            // Lumina a disparut - oprim alarma
            alarma_ldr = false;
            lcd_show_pin(idx);
        }

        // ===== KEYPAD =====
        char key = keypad_scan();
        if (key == 0) { sleep_ms(50); continue; }

        if (key == '#') {
            if (seif_deschis) {
                printf("Seiful e deja deschis!\n");
            } else {
                input[idx] = '\0';
                if (idx == PIN_LENGTH && strcmp(input, PAROLA) == 0) {
                    printf("PAROLA CORECTA!\n");
                    incercari = 0;
                    lcd_show_ok();
                    buzzer_beep(100); sleep_ms(50); buzzer_beep(100);
                    servo_deschide();
                    seif_deschis = true;
                } else {
                    incercari++;
                    printf("PAROLA GRESITA! (%d/%d)\n", incercari, MAX_ATTEMPTS);
                    lcd_show_err();
                    buzzer_beep(500);
                    sleep_ms(1500);

                    if (incercari >= MAX_ATTEMPTS) {
                        incercari = 0;
                        lockout_sequence();
                    } else {
                        lcd_show_pin(0);
                    }
                }
            }
            idx = 0;
            memset(input, 0, sizeof(input));

        } else if (key == '*') {
            if (seif_deschis) {
                printf("Seif inchis.\n");
                servo_inchide();
                seif_deschis = false;
                alarma_ldr   = false;
                lcd_show_closed();
                sleep_ms(1500);
                lcd_show_pin(0);
            } else {
                idx = 0;
                memset(input, 0, sizeof(input));
                lcd_show_pin(0);
                printf("Sters.\n");
            }

        } else if (key == 'D') {
            if (idx > 0) {
                idx--;
                input[idx] = '\0';
                lcd_show_pin(idx);
                printf("Stearsa ultima cifra.\n");
            }

        } else if (idx < PIN_LENGTH && !seif_deschis) {
            input[idx++] = key;
            lcd_show_pin(idx);
            printf("*");
        }

        sleep_ms(50);
    }
}