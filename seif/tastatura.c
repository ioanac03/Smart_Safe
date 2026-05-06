#include "header.h"

const uint row_pins[ROWS] = {16, 17, 18, 19};
const uint col_pins[COLS] = {20, 21, 22, 26};

const char keymap[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

volatile bool tasta_apasata = false;
volatile char ultima_tasta  = 0;

void keypad_low_handler(uint gpio, uint32_t events) {
    for (int r = 0; r < ROWS; r++) {
        gpio_put(row_pins[r], 0);
        sleep_us(5);
        for (int c = 0; c < COLS; c++) {
            if (!gpio_get(col_pins[c])) {
                ultima_tasta  = keymap[r][c];
                tasta_apasata = true;
            }
        }
        gpio_put(row_pins[r], 1);
    }
}

void keypad_init() {
    for (int r = 0; r < ROWS; r++) {
        gpio_init(row_pins[r]);
        gpio_set_dir(row_pins[r], GPIO_OUT);
        gpio_put(row_pins[r], 1); //daca nu merge aici trebuie pus pe 0
    }

    for (int c = 0; c < COLS; c++ ) {
        gpio_init(col_pins[c]);
        gpio_set_dir(col_pins[c], GPIO_IN);
        gpio_pull_up(col_pins[c]);

        //intrerupere pe fond descrescator (Tasta apasata)
        gpio_set_irq_enabled_with_callback(
            col_pins[c],
            GPIO_IRQ_EDGE_FALL,
            true,
            &keypad_low_handler
        );
    }
}

char keypad_scan() {
    if(!tasta_apasata) return 0;

    char k = ultima_tasta;
    tasta_apasata = false;
    ultima_tasta = 0;
    
    //asteapta eliberarea tastei
    sleep_ms(50);
    return k;
}
