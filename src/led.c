#include <avr/io.h>
#include <stdio.h>

#include "led.h"

void enable_led(void) {
  PORTA.OUTSET = PIN1_bm; // enable DISP LATCH
  PORTA.DIRSET = PIN1_bm; // the DISP LATCH as output 
}
