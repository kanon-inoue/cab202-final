#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <avr/io.h>
#include <avr/interrupt.h>

void clock_init(void)
{
    cli(); // clear all intrupt 
    TCB1.CCMP = 3333;
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm;
    sei(); // set all intrupts
}

void buttons_init(void)
{
    PORTA_PIN4CTRL = 0b00001011; // s1
    PORTA_PIN5CTRL = 0b00001011; // s2
    PORTA_PIN6CTRL = 0b00001011; // s3
    PORTA_PIN7CTRL = 0b00001011; // s4
} 

#endif
