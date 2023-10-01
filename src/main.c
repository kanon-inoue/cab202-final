#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>


// #include "uart.h"
#include "timer.h"
// #include "spi.h"
#include "initialization.h"
// #include "sequence.h"
#include "buzzer.h"

int main(void)
{
  uint16_t sequence_length = 1;
  uint16_t score = 0;
  uint16_t playback_delay = 1000;
  
  // for buzzer
  PORTA.DIRSET = PIN1_bm;
  PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
  _delay_ms(3000); // 3 second delay

  clock_init();
  pwm_init();

  while (1)
    {
      // PB0 is connected to buzzeer
      if (elapsed_time <= 4630) {
        // TCA0.SINGLE.CMP0BUF = elapsed_time;
      } else {
        // TCA0.SINGLE.CMP0BUF = 0;
      }
    } // Loop indefinitely
}
