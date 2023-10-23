#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

void button_init(void);
void pwm_init(void);
void stop_buzzer(void);
void timer_init(void);
void clock_init(void);
void potentiometer_init(void);
extern volatile uint8_t pb_debounced_state;