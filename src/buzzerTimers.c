#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

void button_init(void) { 
  // Enable pull-up resistors for PBs
  PORTA_PIN4CTRL |= PORT_PULLUPEN_bm; // s1  PORT_PULLPEN_bm
  PORTA_PIN5CTRL |= PORT_PULLUPEN_bm; // s2   0b00001011
  PORTA_PIN6CTRL |= PORT_PULLUPEN_bm; // s3 0b00001011
  PORTA_PIN7CTRL |= PORT_PULLUPEN_bm; // s4 0b00001011
}

void pwm_init(void) 
{
    PORTB.OUTSET = PIN0_bm; // enable Buzzer 
    PORTB.DIRSET = PIN0_bm; // the Buzzer as output --> TCA0WO0
    PORTA.OUTSET = PIN1_bm; // enable DISP LATCH
    PORTA.DIRSET = PIN1_bm; // the DISP LATCH as output 
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc; // 3.33 Mhz (prescalar = 1)
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // turn on buzzer, WO0, single slope 
    // decide period. before turn off -> PER after turn on --> PERBUF
    TCA0.SINGLE.PER = 1; // for pitch  // CPU_CLK_SPEED / Disired_freq  // 3.33333333333 Mhz / Frequency (360)
    TCA0.SINGLE.CMP0 = 0; // for volume   // buzzer - see schematic    // connects to waveform output 0, whhich is CMP0

    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;  // enable the timer
}

void stop_buzzer(void) {
  TCA0.SINGLE.CMP0BUF = 0;
}

void timer_init(void) {
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc; // 3.33 Mhz (prescalar = 1) // TODO
  TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // turn on buzzer, WO0, single slope 
  // decide period. before turn off -> PER after turn on --> PERBUF
  TCA0.SINGLE.PER = 33333;
  TCA0.SINGLE.CMP0 = 0; // for volume   // buzzer - see schematic    // connects to waveform output 0, whhich is CMP0
  TCA0.SINGLE.CMP1 = 0; // for 
  TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;  // enable the timer
}

void clock_init(void) {
  TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Configure TCB0 in periodic interrupt mode // TODO
  TCB0.INTCTRL |= TCB_CAPT_bm; // CAPT interrupt enable
  TCB0.CCMP = 3333; // Set interval for 1ms (3333 clocks @ 3.3 MHz) 
  TCB0.CTRLA |= TCB_ENABLE_bm; // Enable
}