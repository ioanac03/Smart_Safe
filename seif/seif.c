#include "header.h"
const char PAROLA[] = "1234";
const char PAROLA_ALARMA[] = "9999";
bool pin_gresit = false;

void lockout_sequence() {
    printf("BLOCAT 30 secunde!\n");
    for (int i = LOCKOUT_SEC; i > 0; i--) {
        lcd_show_lockout(i);
        // Buzzer suna in pauza
        gpio_put(BUZZER_PIN, 1); sleep_ms(100);
        gpio_put(BUZZER_PIN, 0); sleep_ms(900);
        printf("Lockout: %d s ramase\n", i);

        char key = keypad_scan();
        if (pin_gresit && key == 'A') {
            alarma_stop();
            alarma_ldr = false;
            printf("Lockout anulat!\n");
            return;
        }
    }
    lcd_show_pin(0);
}

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

    printf("Sistem de securitate pentru seif pornit. Asteptare input...\n");

    while (true) { 
        if (!seif_deschis && ldr_lumina_detectata()) {            
            if (!alarma_ldr) {                
                alarma_ldr = true;                
                printf("ALARMA: lumina detectata in seif inchis!\n");                
                lcd_show_alarm();
                alarma_start();       
            }

            char key = keypad_scan();
            if (key == 'A') {
                alarma_stop();
                alarma_ldr = false;
                printf("Alarma oprita!\n");
                lcd_show_pin(idx);
                continue;
            }

        continue;      
        } 

        char key = keypad_scan();
        printf("Tasta scanata: %d\n", key);        
        if (key == 0) { 
            sleep_ms(50); 
            continue; 
        }        
        printf("Scanare tasta...\n");
        if (key == '#') {            
            printf("Tasta # apasata\n");
            if (seif_deschis) {                
                printf("Seiful e deja deschis!\n");            
            } else {                
                input[idx] = '\0';                
                if (idx == PIN_LENGTH && strcmp(input, PAROLA) == 0) {                    
                    printf("PAROLA CORECTA!\n"); 
                    pin_gresit = false;                   
                    incercari = 0;                    
                    lcd_show_ok();                    
                    buzzer_beep(100); 
                    sleep_ms(50); 
                    buzzer_beep(100);                    
                    servo_deschide();    
                    if (alarma_ldr) {
                        alarma_stop();
                        alarma_ldr = false;
                    }
                    seif_deschis = true;                
                } else {                    
                    incercari++;                    
                    printf("PAROLA GRESITA! (%d/%d)\n", incercari, MAX_ATTEMPTS);                    
                    lcd_show_err();                    
                    buzzer_beep(500);   
                    pin_gresit = true;                 
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
            printf("Tasta %c apasata\n", key);
            lcd_show_pin(idx);
            printf("*");
        }

        sleep_ms(50);
    }
}
         /*if (!seif_deschis && ldr_lumina_detectata()) {
            if (!alarma_ldr) {
                alarma_ldr = true;
                printf("ALARMA: lumina detectata in seif inchis!\n");
                lcd_show_alarm();
                alarma_start(); 
            } else if (alarma_ldr && !ldr_lumina_detectata()) {

                char key = keypad_scan();
                if (key == 0) { sleep_ms(50); continue; }
                printf("Scanare tasta...\n");
                
                if (key == '#') {
                    printf("Tasta # apasata\n");
                    input[idx] = '\0';
                    if (idx == PIN_LENGTH && strcmp(input, PAROLA_ALARMA) == 0) {
                        alarma_stop();
                        alarma_ldr = false;
                        alarma_stop();
                        lcd_show_pin(0);
                        printf("Alarma oprita prin PIN secundar!\n");
                    } else if (key == 'D') {
                        if (idx > 0) {
                            idx--;
                            input[idx] = '\0';
                            lcd_show_pin(idx);
                            printf("Stearsa ultima cifra.\n");
                        }

                    } else if (idx < PIN_LENGTH && !seif_deschis) {
                        printf("Tasta %c apasata\n", key);
                        input[idx++] = key;
                        lcd_show_pin(idx);
                        printf("*");
                    }
                }
            }
        }

        char key = keypad_scan();
        if (key == 0) { sleep_ms(50); continue; }
        printf("Scanare tasta...\n");
        if (key == '#') {
            printf("Tasta # apasata\n");
            if (seif_deschis) {
                printf("Seiful e deja deschis!\n");
            } else {
                input[idx] = '\0';
                if (idx == PIN_LENGTH && strcmp(input, PAROLA) == 0) {
                    printf("PAROLA CORECTA!\n");
                    incercari = 0;
                    alarma_ldr = false;
                    alarma_stop();
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
                         if (!alarma_ldr) lcd_show_pin(0);
                        else lcd_show_alarm();
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
                lcd_show_closed();
                sleep_ms(1500);
                lcd_show_pin(0);
            } else {
                idx = 0;
                memset(input, 0, sizeof(input));
                if (!alarma_ldr) lcd_show_pin(0);
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
            printf("Tasta %c apasata\n", key);
            input[idx++] = key;
            lcd_show_pin(idx);
            printf("*");
        }

        sleep_ms(50);*/
