#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <hardware/spi.h>
#include <hardware/adc.h>
#include <hardware/dma.h>
#include <hardware/irq.h>


// PINI LCD
#define LCD_SPI      spi0
#define LCD_CLK      2
#define LCD_MOSI     3
#define LCD_CS       5
#define LCD_DC       4
#define LCD_RST      6 
#define LCD_BL       7
#define LCD_W 128
#define LCD_H 160

//PINI COMPONENTE
#define SERVO_PIN    15
#define BUZZER_PIN   14
#define LDR_PIN      27   // GP27 = ADC1
#define LDR_ADC_CH   1    // canal ADC pentru GP27

//PINI TASTATURA
#define ROWS 4
#define COLS 4
#define PIN_LENGTH 4

//CULORI
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define ORANGE  0xFD20

//PRAGURI SI CONSTANTE
#define LDR_THRESHOLD  2000
#define MAX_ATTEMPTS   3
#define LOCKOUT_SEC    30

extern volatile bool alarma_ldr;
extern volatile bool tasta_apasata;
extern volatile char ultima_tasta;
extern bool seif_deschis;
extern int  incercari;

extern const uint row_pins[ROWS];
extern const uint col_pins[COLS];
extern const char keymap[ROWS][COLS];
extern const char PAROLA[];

//LCD

void lcd_init();
void lcd_fill(uint16_t color);
void lcd_draw_char(uint8_t x, uint8_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale);
void lcd_draw_string(uint8_t x, uint8_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t scale);
void lcd_show_pin(int idx);
void lcd_show_ok();
void lcd_show_err();
void lcd_show_closed();
void lcd_show_alarm();
void lcd_show_lockout(int sec_ramase);


//SERVO
void servo_init();
void servo_deschide();
void servo_inchide();

//BUZZER
void buzzer_init();
void buzzer_beep(int ms);
void buzzer_alarm(int durata_ms);

//SENZOR
void ldr_init();
uint16_t ldr_read();
bool ldr_lumina_detectata();

//TASTATURA
void keypad_init();
char keypad_scan();
void keypad_low_handler(uint gpio, uint32_t events);

#endif

