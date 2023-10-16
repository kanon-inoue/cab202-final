#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "project.h"
#include "buzzer.h"


int main(void)
{
  init();
  // pwm_init();
  adc_init();
  while (1)
  {
    playback_delay = ADC0.RESULT;
    // TCA0.SINGLE.CMP1BUF = ((uint32_t)9259 * result) >> 8;

  }
       // ; // Loop indefinitely
}
