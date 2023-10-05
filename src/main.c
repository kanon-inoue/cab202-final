#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "timer.h"
#include "initialization.h"
#include "buzzer.h"

#include "sequence.h"
//#include "uart.h"
//#include "spi.h"

#include "qutyio.h"

// CMP0 is volume 
// PER is pitch

int main(void)
{
  uint32_t MASK = 0xE2023CAB;
  uint32_t STATE_LFSR = 0x11186267;
  uint16_t sequence_length = 1;
  uint16_t score = 0;
  uint16_t playback_delay = 1000;

  display_init();  // Configures pins PA1, PC0, and PC2, to drive the 7-segment display
  display_on(); // Drives DISP EN net HIGH
  serial_init();

  printf("the main starts runnnig! \n");

  
  // for buzzer
  PORTA.DIRSET = PIN1_bm; // DISP LATCH??
  PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // button3?
  _delay_ms(3000); // 3 second delay

  clock_init();
  //buttons_init();
  //printf("The stopwatch is done");
  pwm_init();

  while (1)
    {
      // duty cycle = ((255 - ADC0/RESULT)/255) 
        // CMP1BUF = 9259 * ((255 - ADC0/RESULT) / 255) 
        // CMP1BUF = (9259 * ((255 - ADC0/RESULT)) / 256 
        // CMP1BUF = (9259 * (255 - ADC0/RESULT)) >> 8
        // CMP1BUF = ((uint32_t)9259 * (255 - ADC0/RESULT)) >> 8
      //uint32_t result = 255 - ADC0.RESULT;
      //TCA0.SINGLE.CMP1BUF = ((uint32_t)9259 * result) >> 8;
      printf("elapsed time is %u \n", elapsed_time);
      // PB0 is connected to buzzeer
      if (elapsed_time <= 4630) {
        TCA0.SINGLE.CMP0BUF = elapsed_time;
      } else {
        TCA0.SINGLE.CMP0BUF = 0;
      }
    } // Loop indefinitely
}
