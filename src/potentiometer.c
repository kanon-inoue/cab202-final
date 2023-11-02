#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

void potentiometer_init(void) {
  // configure ADC0 in 8-bit single conversion mode.
  ADC0.CTRLA = ADC_ENABLE_bm; // enable adc
  ADC0.CTRLB  = ADC_PRESC_DIV2_gc; // /2 prescaler
  ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc; // or ADC0.CTRLC = 0b00100000 | ADC_REFSEL_VDD_gc;
  ADC0.CTRLE = 64; // sample duration of 64
  ADC0.CTRLF = ADC_FREERUN_bm; // free running, left adjust or ADC0.CTRLF = ADC_FREERUN_bm | ADC_LEFTADJ_bm;
  ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
  ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc | ADC_START_IMMEDIATE_gc; // 8 bit resolusion, single ended
}

uint32_t get_new_playback_delay(uint32_t adc_result) { // we want duty cycle will be 0% and 100% and new result: 0 - 255
  return ((((adc_result+1)*1750UL) >> 8) + 250);
}