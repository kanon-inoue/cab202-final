#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

#include "qutyio.h"

// E (high) == 467 * 2^(-5/12) == 349.85 ~= 350
// C# == 467 * 2^(-8/12) == 294.19 ~= 294
// A == 467
// E (low) == 467 * 2^(-17/12) == 174.93 ~= 175

void pwm_init(void) 
{
    printf("the pwm is called \n");
    PORTB.OUTSET = PIN1_bm; // enable? DISPEN?
    PORTB.DIRSET = PIN1_bm; // the DISPEN as output 
    PORTB.DIRSET = PIN0_bm; //for display en???  for buzzer?
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc; // 3.33 Mhz (prescalar = 1)
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm; // single slope, WO1 (ex8.4 and WO0)

    TCA0.SINGLE.PERBUF = 9259; // for pitch  // CPU_CLK_SPEED / Disired_freq  // 3.33333333333 Mhz / Frequency (360)
    TCA0.SINGLE.CMP0BUF = 0; // for volume   // buzzer - see schematic    // connects to waveform output 0, whhich is CMP0
    
    // DISPEN - see schematic 
    // connects to waveform output 1, which is CMP1
    // % duty cycle * PER (0.17 * 9259 )
    TCA0.SINGLE.CMP1 = 1574; //for screen? 

    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;  // enable the timer

    printf("pwm is done \n");
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