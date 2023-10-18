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

  while (1)
  {

    
    // ADC0.RESULT * x + 250
    // 255 * x + 250 = 2000
    // x == 6.8627


    //playback_delay = ADC0.RESULT;
    // TCA0.SINGLE.CMP1BUF = ((uint32_t)9259 * result) >> 8;

  }
       // ; // Loop indefinitely
}

uint16_t get_new_playback_delay(uint16_t adc_result) {
  return (adc_result * 6.8627) + 250;
}