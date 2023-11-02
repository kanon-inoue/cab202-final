#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h> // contains prototypes for functions that read input and write output
#include <stdint.h> // read input
#include <string.h>
#include "led.h"

uint16_t dec_freq() {
  E_high = E_high * 2;
  C = C * 2;
  A = A * 2;
  E_low = E_low * 2;
}

uint16_t inc_freq() {
  E_high = E_high / 2;
  C = C / 2;
  A = A / 2;
  E_low = E_low / 2;
}