#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/adc.h"

// ===== PINI KEYPAD =====
#define ROWS 4
#define COLS 4
#define PIN_LENGTH 4

// ===== PINI LCD =====
#define LCD_SPI      spi0
#define LCD_CLK      2
#define LCD_MOSI     3
#define LCD_CS       5
#define LCD_DC       4
#define LCD_RST      6
#define LCD_BL       7

// ===== PINI COMPONENTE =====
#define SERVO_PIN    15
#define BUZZER_PIN   14
#define LDR_PIN      27   // GP27 = ADC1
#define LDR_ADC_CH   1    // canal ADC pentru GP27

#define LCD_W 128
#define LCD_H 160

// Prag lumina - valoare ADC sub acest prag = lumina detectata
// (cu pull-up: lumina => rezistenta scade => tensiune scade)
#define LDR_THRESHOLD 2000

// Incercari maxime inainte de blocare
#define MAX_ATTEMPTS  3
#define LOCKOUT_SEC   30

const uint row_pins[ROWS] = {16, 17, 18, 19};
const uint col_pins[COLS] = {20, 21, 22, 26};

const char keymap[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

const char PAROLA[] = "1234";

// ===== CULORI =====
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define ORANGE  0xFD20

// ===== FONT 5x7 =====
static const uint8_t font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x00,0x00,0x5F,0x00,0x00}, // '!'
    {0x00,0x07,0x00,0x07,0x00}, // '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
    {0x23,0x13,0x08,0x64,0x62}, // '%'
    {0x36,0x49,0x55,0x22,0x50}, // '&'
    {0x00,0x05,0x03,0x00,0x00}, // '''
    {0x00,0x1C,0x22,0x41,0x00}, // '('
    {0x00,0x41,0x22,0x1C,0x00}, // ')'
    {0x08,0x2A,0x1C,0x2A,0x08}, // '*'
    {0x08,0x08,0x3E,0x08,0x08}, // '+'
    {0x00,0x50,0x30,0x00,0x00}, // ','
    {0x08,0x08,0x08,0x08,0x08}, // '-'
    {0x00,0x60,0x60,0x00,0x00}, // '.'
    {0x20,0x10,0x08,0x04,0x02}, // '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    {0x00,0x36,0x36,0x00,0x00}, // ':'
    {0x00,0x56,0x36,0x00,0x00}, // ';'
    {0x00,0x08,0x14,0x22,0x41}, // '<'
    {0x14,0x14,0x14,0x14,0x14}, // '='
    {0x41,0x22,0x14,0x08,0x00}, // '>'
    {0x02,0x01,0x51,0x09,0x06}, // '?'
    {0x32,0x49,0x79,0x41,0x3E}, // '@'
    {0x7E,0x11,0x11,0x11,0x7E}, // 'A'
    {0x7F,0x49,0x49,0x49,0x36}, // 'B'
    {0x3E,0x41,0x41,0x41,0x22}, // 'C'
    {0x7F,0x41,0x41,0x22,0x1C}, // 'D'
    {0x7F,0x49,0x49,0x49,0x41}, // 'E'
    {0x7F,0x09,0x09,0x09,0x01}, // 'F'
    {0x3E,0x41,0x49,0x49,0x7A}, // 'G'
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H'
    {0x00,0x41,0x7F,0x41,0x00}, // 'I'
    {0x20,0x40,0x41,0x3F,0x01}, // 'J'
    {0x7F,0x08,0x14,0x22,0x41}, // 'K'
    {0x7F,0x40,0x40,0x40,0x40}, // 'L'
    {0x7F,0x02,0x04,0x02,0x7F}, // 'M'
    {0x7F,0x04,0x08,0x10,0x7F}, // 'N'
    {0x3E,0x41,0x41,0x41,0x3E}, // 'O'
    {0x7F,0x09,0x09,0x09,0x06}, // 'P'
    {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'
    {0x7F,0x09,0x19,0x29,0x46}, // 'R'
    {0x46,0x49,0x49,0x49,0x31}, // 'S'
    {0x01,0x01,0x7F,0x01,0x01}, // 'T'
    {0x3F,0x40,0x40,0x40,0x3F}, // 'U'
    {0x1F,0x20,0x40,0x20,0x1F}, // 'V'
    {0x3F,0x40,0x38,0x40,0x3F}, // 'W'
    {0x63,0x14,0x08,0x14,0x63}, // 'X'
    {0x07,0x08,0x70,0x08,0x07}, // 'Y'
    {0x61,0x51,0x49,0x45,0x43}, // 'Z'
    {0x00,0x7F,0x41,0x41,0x00}, // '['
    {0x02,0x04,0x08,0x10,0x20}, // '\'
    {0x00,0x41,0x41,0x7F,0x00}, // ']'
    {0x04,0x02,0x01,0x02,0x04}, // '^'
    {0x40,0x40,0x40,0x40,0x40}, // '_'
    {0x00,0x01,0x02,0x04,0x00}, // '`'
    {0x20,0x54,0x54,0x54,0x78}, // 'a'
    {0x7F,0x48,0x44,0x44,0x38}, // 'b'
    {0x38,0x44,0x44,0x44,0x20}, // 'c'
    {0x38,0x44,0x44,0x48,0x7F}, // 'd'
    {0x38,0x54,0x54,0x54,0x18}, // 'e'
    {0x08,0x7E,0x09,0x01,0x02}, // 'f'
    {0x0C,0x52,0x52,0x52,0x3E}, // 'g'
    {0x7F,0x08,0x04,0x04,0x78}, // 'h'
    {0x00,0x44,0x7D,0x40,0x00}, // 'i'
    {0x20,0x40,0x44,0x3D,0x00}, // 'j'
    {0x7F,0x10,0x28,0x44,0x00}, // 'k'
    {0x00,0x41,0x7F,0x40,0x00}, // 'l'
    {0x7C,0x04,0x18,0x04,0x78}, // 'm'
    {0x7C,0x08,0x04,0x04,0x78}, // 'n'
    {0x38,0x44,0x44,0x44,0x38}, // 'o'
    {0x7C,0x14,0x14,0x14,0x08}, // 'p'
    {0x08,0x14,0x14,0x18,0x7C}, // 'q'
    {0x7C,0x08,0x04,0x04,0x08}, // 'r'
    {0x48,0x54,0x54,0x54,0x20}, // 's'
    {0x04,0x3F,0x44,0x40,0x20}, // 't'
    {0x3C,0x40,0x40,0x20,0x7C}, // 'u'
    {0x1C,0x20,0x40,0x20,0x1C}, // 'v'
    {0x3C,0x40,0x30,0x40,0x3C}, // 'w'
    {0x44,0x28,0x10,0x28,0x44}, // 'x'
    {0x0C,0x50,0x50,0x50,0x3C}, // 'y'
    {0x44,0x64,0x54,0x4C,0x44}, // 'z'
};

// ===== ST7735 COMENZI =====
#define ST7735_SWRESET 0x01
#define ST7735_SLPOUT  0x11
#define ST7735_NORON   0x13
#define ST7735_INVOFF  0x20
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_MADCTL  0x36
#define ST7735_COLMOD  0x3A

// ===== LCD LOW LEVEL =====
static inline void lcd_cs_select()   { gpio_put(LCD_CS, 0); }
static inline void lcd_cs_deselect() { gpio_put(LCD_CS, 1); }
static inline void lcd_dc_cmd()      { gpio_put(LCD_DC, 0); }
static inline void lcd_dc_data()     { gpio_put(LCD_DC, 1); }

void lcd_write_byte(uint8_t val) { spi_write_blocking(LCD_SPI, &val, 1); }

void lcd_cmd(uint8_t cmd) {
    lcd_cs_select(); lcd_dc_cmd();
    lcd_write_byte(cmd); lcd_cs_deselect();
}

void lcd_data(uint8_t data) {
    lcd_cs_select(); lcd_dc_data();
    lcd_write_byte(data); lcd_cs_deselect();
}

void lcd_init() {
    spi_init(LCD_SPI, 20 * 1000 * 1000);
    gpio_set_function(LCD_CLK,  GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);

    gpio_init(LCD_CS);  gpio_set_dir(LCD_CS,  GPIO_OUT); gpio_put(LCD_CS,  1);
    gpio_init(LCD_DC);  gpio_set_dir(LCD_DC,  GPIO_OUT); gpio_put(LCD_DC,  1);
    gpio_init(LCD_RST); gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_init(LCD_BL);  gpio_set_dir(LCD_BL,  GPIO_OUT); gpio_put(LCD_BL,  1);

    gpio_put(LCD_RST, 0); sleep_ms(100);
    gpio_put(LCD_RST, 1); sleep_ms(100);

    lcd_cmd(ST7735_SWRESET); sleep_ms(150);
    lcd_cmd(ST7735_SLPOUT);  sleep_ms(150);
    lcd_cmd(ST7735_COLMOD);  lcd_data(0x05);
    lcd_cmd(ST7735_MADCTL);  lcd_data(0x00);
    lcd_cmd(ST7735_NORON);   sleep_ms(10);
    lcd_cmd(ST7735_INVOFF);
    lcd_cmd(ST7735_DISPON);  sleep_ms(10);
}

void lcd_set_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    lcd_cmd(ST7735_CASET);
    lcd_data(0x00); lcd_data(x0); lcd_data(0x00); lcd_data(x1);
    lcd_cmd(ST7735_RASET);
    lcd_data(0x00); lcd_data(y0); lcd_data(0x00); lcd_data(y1);
    lcd_cmd(ST7735_RAMWR);
}

void lcd_fill(uint16_t color) {
    lcd_set_window(0, 0, LCD_W - 1, LCD_H - 1);
    lcd_cs_select(); lcd_dc_data();
    uint8_t buf[2] = { color >> 8, color & 0xFF };
    for (int i = 0; i < LCD_W * LCD_H; i++)
        spi_write_blocking(LCD_SPI, buf, 2);
    lcd_cs_deselect();
}

void lcd_draw_char(uint8_t x, uint8_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale) {
    if (c < 0x20 || c > 0x7A) c = '?';
    const uint8_t *glyph = font5x7[c - 0x20];
    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 7; row++) {
            uint16_t color = (glyph[col] & (1 << row)) ? fg : bg;
            lcd_set_window(
                x + col * scale, y + row * scale,
                x + col * scale + scale - 1, y + row * scale + scale - 1
            );
            lcd_cs_select(); lcd_dc_data();
            for (int s = 0; s < scale * scale; s++) {
                uint8_t buf[2] = { color >> 8, color & 0xFF };
                spi_write_blocking(LCD_SPI, buf, 2);
            }
            lcd_cs_deselect();
        }
    }
}

void lcd_draw_string(uint8_t x, uint8_t y, const char *str,
                     uint16_t fg, uint16_t bg, uint8_t scale) {
    while (*str) {
        lcd_draw_char(x, y, *str++, fg, bg, scale);
        x += 6 * scale;
        if (x + 6 * scale > LCD_W) break;
    }
}

// ===== AFISARE LCD =====
void lcd_show_pin(int idx) {
    lcd_fill(BLACK);
    lcd_draw_string(10, 30, "PIN:", WHITE, BLACK, 2);
    char stars[PIN_LENGTH + 1];
    memset(stars, ' ', PIN_LENGTH);
    stars[PIN_LENGTH] = '\0';
    for (int i = 0; i < idx; i++) stars[i] = '*';
    lcd_draw_string(10, 70, stars, YELLOW, BLACK, 2);
}

void lcd_show_ok() {
    lcd_fill(GREEN);
    lcd_draw_string(10, 60, "PAROLA",   BLACK, GREEN, 2);
    lcd_draw_string(10, 90, "CORECTA!", BLACK, GREEN, 2);
}

void lcd_show_err() {
    lcd_fill(RED);
    lcd_draw_string(10, 60, "PAROLA",  WHITE, RED, 2);
    lcd_draw_string(10, 90, "GRESITA!", WHITE, RED, 2);
}

void lcd_show_closed() {
    lcd_fill(BLUE);
    lcd_draw_string(15, 70, "INCHIS", WHITE, BLUE, 2);
}

void lcd_show_alarm() {
    lcd_fill(RED);
    lcd_draw_string(5,  40, "ALARMA!", WHITE, RED, 2);
    lcd_draw_string(5,  75, "EFRACTIE", WHITE, RED, 2);
    lcd_draw_string(5, 110, "DETECTATA", WHITE, RED, 2);
}

void lcd_show_lockout(int sec_ramase) {
    lcd_fill(ORANGE);
    lcd_draw_string(5,  30, "BLOCAT!", BLACK, ORANGE, 2);
    lcd_draw_string(5,  60, "Pauza:", BLACK, ORANGE, 2);
    // Afisam secundele ramase
    char buf[8];
    snprintf(buf, sizeof(buf), "%d s", sec_ramase);
    lcd_draw_string(10, 95, buf, BLACK, ORANGE, 2);
}

// ===== BUZZER =====
void buzzer_init() {
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

void buzzer_beep(int ms) {
    gpio_put(BUZZER_PIN, 1);
    sleep_ms(ms);
    gpio_put(BUZZER_PIN, 0);
}

// Alarma continua cu intrerupere la fiecare 500ms
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

// ===== KEYPAD =====
void keypad_init() {
    for (int r = 0; r < ROWS; r++) {
        gpio_init(row_pins[r]);
        gpio_set_dir(row_pins[r], GPIO_OUT);
        gpio_put(row_pins[r], 1);
    }
    for (int c = 0; c < COLS; c++) {
        gpio_init(col_pins[c]);
        gpio_set_dir(col_pins[c], GPIO_IN);
        gpio_pull_up(col_pins[c]);
    }
}

char keypad_scan() {
    for (int r = 0; r < ROWS; r++) {
        gpio_put(row_pins[r], 0);
        sleep_us(10);
        for (int c = 0; c < COLS; c++) {
            if (!gpio_get(col_pins[c])) {
                while (!gpio_get(col_pins[c])) { sleep_ms(10); }
                gpio_put(row_pins[r], 1);
                return keymap[r][c];
            }
        }
        gpio_put(row_pins[r], 1);
    }
    return 0;
}

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