#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "timer.h"
#include "initialization.h"
#include "buzzer.h"

//#include "uart.h"
//#include "spi.h"
//#include "sequence.h"

#include "qutyio.h"

// CMP0 is volume 
// PER is pitch

int main(void)
{
  // Configures pins PA1, PC0, and PC2, to drive the 7-segment display
  display_init();
  // Drives DISP EN net HIGH
  display_on();

  serial_init();

  uint16_t sequence_length = 1;
  uint16_t score = 0;
  uint16_t playback_delay = 0;
  
  // for buzzer
  PORTA.DIRSET = PIN1_bm; // DISP LATCH??
  PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // button3?
  _delay_ms(3000); // 3 second delay

  clock_init();
  //buttons_init();
  pwm_init();

  while (1)
    {
      // PB0 is connected to buzzeer
      if (elapsed_time >= 4630) {
        TCA0.SINGLE.CMP0BUF = elapsed_time;
      } else {
        TCA0.SINGLE.CMP0BUF = 0;
      }
    } // Loop indefinitely
}
