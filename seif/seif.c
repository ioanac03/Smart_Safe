#include "header.h"
const char PAROLA[] = "1234";

void lockout_sequence() {
    if (alarma_ldr) {
        alarma_stop();
    }

    printf("BLOCAT 30 secunde!\n");
    for (int i = LOCKOUT_SEC; i > 0; i--) {
        lcd_show_lockout(i);
        // Buzzer suna in pauza
        gpio_put(BUZZER_PIN, 1); sleep_ms(100);
        gpio_put(BUZZER_PIN, 0); sleep_ms(900);
        printf("Lockout: %d s ramase\n", i);

    }

    if(alarma_ldr){
        alarma_start();
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
    int closed = 0;

    while (true) { 
        if (!seif_deschis && ldr_lumina_detectata()) {            
            if (!alarma_ldr) {                
                alarma_ldr = true;                
                printf("ALARMA: lumina detectata in seif inchis!\n");                
                lcd_show_alarm();
                alarma_start();
                if (closed == 0){
                    servo_inchide();
                    closed = 1;
                }
                       
            }
        } 

        char key = keypad_scan();   
        if (key == 0) { 
            sleep_ms(50); 
            continue; 
        }        
        if (key == '#') {            
            if (seif_deschis) {                
                printf("Seiful e deja deschis!\n");      
                //verificam daca parola este corecta      
            } else {                
                input[idx] = '\0';                
                if (idx == PIN_LENGTH && strcmp(input, PAROLA) == 0) {                    
                    printf("PAROLA CORECTA!\n");              
                    incercari = 0;                    
                    lcd_show_ok();                 
                    buzzer_beep(100); 
                    sleep_ms(50); 
                    if (alarma_ldr) {
                        alarma_stop();
                        alarma_ldr = false;
                        closed = 0;
                    }
                    buzzer_beep(100);                    
                    servo_deschide();    
                    seif_deschis = true;                
                } else {   
                    //pt brute-force                 
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
            //dorinta de a inchide seiful
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
            //tasta de delete
            if (idx > 0) {
                idx--;
                input[idx] = '\0';
                lcd_show_pin(idx);
                printf("Stearsa ultima cifra.\n");
            }

        } else if (idx < PIN_LENGTH && !seif_deschis) {
            //introducerea pin-ului pana la 4 cifre
            input[idx++] = key;
            lcd_show_pin(idx);
            printf("*");
        }

        sleep_ms(50);
    }
}
       