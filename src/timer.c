#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "qutyio.h"

uint32_t elapsed_time = 0;
uint8_t is_counting = 1;

ISR(TCB1_INT_vect){
  if (is_counting == 1) 
  {
    // This makes the noise
    // TCA0.SINGLE.CMP0BUF = elapsed_time;
    if (elapsed_time < 1020) // 1020= 64*(15+15)/16
    {
      elapsed_time = elapsed_time + 1;
    }
  }
  display_hex(elapsed_time/4);
  TCB1.INTFLAGS = 0b00000001;
}



    // to start the timer 
    // is_counting = 1;
    // VPORTA_INTFLAGS = PIN4_bm; pin4 is s1

    // to stop the timer
    // VPORTA_INTFLAGS = PIN5_bm; pin5 is s2

    // to reset the timer to 0
    // elapsed_time = 0;
    // VPORTA_INTFLAGS = PIN7_bm; pin7 is s4



