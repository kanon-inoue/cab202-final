#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

void pwm_init(void) 
{
    //PORTB.OUTSET = PIN0_bm; // enable Buzzer 
    PORTB.DIRSET = PIN0_bm; // the Buzzer as output --> TCA0WO0
    // PORTA.DIRSET = PIN1_bm; // enable led desplay 
    // PORTA.OUTSET = PIN1_bm; // enable DISP LATCH
    PORTA.DIRSET = PIN1_bm; // the DISP LATCH as output 

    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc; // 3.33 Mhz (prescalar = 1)
    // TCA0.SINGLE.CTRLB = (TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP0EN_bm); // single slope, WO1
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // turn on buzzer, WO0, single slope 

    // decide period. before turn off -> PER after turn on --> PERBUF
    TCA0.SINGLE.PERBUF = 9259u; // for pitch  // CPU_CLK_SPEED / Disired_freq  // 3.33333333333 Mhz / Frequency (360)
    TCA0.SINGLE.CMP0BUF = 0; // for volume   // buzzer - see schematic    // connects to waveform output 0, whhich is CMP0
    // TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PER >> 1; // per devide by 2

    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;  // enable the timer
}

void adc_init(void) 
{
    ADC0.CTRLA = ADC_ENABLE_bm;
    ADC0.CTRLB= ADC_PRESC_DIV2_gc;
    ADC0.CTRLC = 0b00100000 | ADC_REFSEL_VDD_gc;
    ADC0.CTRLE = 64;
    ADC0.CTRLF = ADC_FREERUN_bm;
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc |ADC_START_IMMEDIATE_gc;
}