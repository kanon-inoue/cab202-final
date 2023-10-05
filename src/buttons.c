#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"

ISR(PORTA_PORT_vect)
{
  // start stopwatch
  if (VPORTA.INTFLAGS & PIN4_bm) 
  {
    is_counting = 1;
    VPORTA_INTFLAGS = PIN4_bm;
  } 
  // stop it
  if (VPORTA.INTFLAGS & PIN5_bm) 
  {
    is_counting = 0;
    VPORTA_INTFLAGS = PIN5_bm;
  }
  // reset it to 0
  if (VPORTA.INTFLAGS & PIN7_bm) 
  {
    elapsed_time = 0;
    VPORTA_INTFLAGS = PIN7_bm;
  }
}
