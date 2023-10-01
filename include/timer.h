#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

extern uint16_t elapsed_time;
extern uint8_t is_counting;

void timer_init(void);
