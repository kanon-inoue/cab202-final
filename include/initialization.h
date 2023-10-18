#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <avr/io.h>
#include <avr/interrupt.h>

void clock_init(void)
{
    // Configure timer for PB sampling
    cli(); // clear all intrupt 
    TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Configure TCB0 in periodic interrupt mode
    TCB0.INTCTRL |= TCB_CAPT_bm; // CAPT interrupt enable
    TCB0.CCMP = 3333; // Set interval for 1ms (3333 clocks @ 3.3 MHz) 
    TCB0.CTRLA |= TCB_ENABLE_bm; // Enable
    sei(); // set all intrupts
}

void buttons_init(void)
{
    // Enable pull-up resistors for PBs
    PORTA_PIN4CTRL |= 0b00001011; // s1  PORT_PULLPEN_bm
    PORTA_PIN5CTRL |= 0b00001011; // s2   
    PORTA_PIN6CTRL |= 0b00001011; // s3 
    PORTA_PIN7CTRL |= 0b00001011; // s4   
} 

#endif
