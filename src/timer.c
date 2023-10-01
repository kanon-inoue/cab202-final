#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

#include <spi.h>

void timer_init(void)
{
    cli();
    TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Configure TCB0 in periodic interrupt mode
    TCB0.CCMP = 3333;                // Set interval for 1ms (3333 clocks @ 3.3 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;      // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;      // Enable
    sei();
}

ISR(TCB0_INT_vect)
{
    /** CODE: Write your code for Ex 9.3 within this ISR. */
    // n11186267
    // LSH is 1 -> 11101011
    // RHS is 1 -> 01101011
    static int left = 1;
    if (left == 1) {
        spi_write(0b11101011);
        left = 0;
    } else {
        spi_write(0b01101011);
        left = 1;
    }
    TCB0.INTFLAGS = TCB_CAPT_bm;
}