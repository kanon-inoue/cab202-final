#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

#include "timer.h"


void pwm_init(void) 
{
    // for ex8.4
    PORTB.OUTSET = PIN1_bm;

    // the DISPEN as output 
    PORTB.DIRSET = PIN1_bm;

    PORTB.DIRSET = PIN0_bm;

    // 3.33 Mhz (prescalar = 1)
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc;
    // single slope, WO1 (ex8.4 and WO0)
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm;

    // CPU_CLK_SPEED / Disired_freq
    // 3.33333333333 Mhz / Frequency (360)
    TCA0.SINGLE.PER = 9259;

    // buzzer - see schematic 
    // connects to waveform output 0, whhich is CMP0
    TCA0.SINGLE.CMP0 = 0; // ex 8.4
    
    // DISPEN - see schematic 
    // connects to waveform output 1, which is CMP1
    // % duty cycle * PER (0.17 * 9259 )
    TCA0.SINGLE.CMP1 = 1574;

    // enable the timer
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
}

void adc_init() 
{
    ADC0.CTRLA = ADC_ENABLE_bm;
    ADC0.CTRLB= ADC_PRESC_DIV2_gc;
    ADC0.CTRLC = 0b00100000 | ADC_REFSEL_VDD_gc;
    ADC0.CTRLE = 64;
    ADC0.CTRLF = ADC_FREERUN_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc |ADC_START_IMMEDIATE_gc;

}