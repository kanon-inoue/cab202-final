// As this file contains function definitions, we must use include
// guards to prevent this file from being included multiple times.
#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <avr/io.h>
#include <avr/interrupt.h>

/** EX: 7.0

When configuring hardware peripherals to generate interrupts, it
is important to ensure that interrupts cannot be raised by other
peripherals while this configuration is taking place.

TASK: Disable interrupts globally before configuring the TCB1
peripheral and re-enable interrupts globally afterwards.

HINT: Use the cli() and sei() functions provided by avr/interrupt.h.
*/

void clock_init(void)
{
    /** CODE: Write your code for Ex 7.0 within this function. */
    cli(); // clear all intrupt 

    // TCB1 is a 16-bit timer that has been configured to generate
    // an interrupt every 1/64 of a second.
    TCB1.CCMP = 52083;
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm;

    sei(); // set all intrupts
}

/** EX: 7.1

The stopwatch will be controlled by pushbuttons S1, S2, and S4.

TASK: Write code to configure the pins connected to these pushbuttons
to generate an interrupt on falling edges.

HINT: See datasheet Section 17.4 Register Summary - PORTx on p. 153,
and Section 17.5.12 Pin n Control on p. 165.
*/

void buttons_init(void)
{
    /** CODE: Write your code for Ex 7.1 within this function. */
    PORTA_PIN4CTRL = 0b00001011;
    PORTA_PIN5CTRL = 0b00001011;
    PORTA_PIN7CTRL = 0b00001011;
}

#endif
