#include "header.h"

void servo_init(){
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_set_clkdiv(slice, 64.0f);
    pwm_set_wrap(slice, 39062);
    pwm_set_enabled(slice, true);
}

static void servo_set_angle(int angle){
    uint slice   = pwm_gpio_to_slice_num(SERVO_PIN);
    uint channel = pwm_gpio_to_channel(SERVO_PIN);
    uint pulse = 1953 + (angle * 1953) / 180;
    pwm_set_chan_level(slice, channel, pulse);
}

void servo_deschide() { servo_set_angle(90); }
void servo_inchide()  { servo_set_angle(0);  }