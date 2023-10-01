#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "uart.h"
#include "timer.h"
#include "spi.h"
#include "initialization.h"
#include "sequence.h"
#include "buzzer.h"

int main(void)
{

  
  PORTA.DIRSET = PIN1_bm;
  PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
  _delay_ms(3000); // 3 second delay

  pwm_init();

  while (1)
    {
        // PB0 is connected to buzzeer
        // if (ADC0.RESULT >= 224) {
            TCA0.SINGLE.CMP0BUF = 4630;
        // } else {
            // TCA0.SINGLE.CMP0BUF = 0;
        // }
    } // Loop indefinitely
}