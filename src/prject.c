#include <avr/io.h>
#include <stdio.h>

#include "timer.h"
#include "initialization.h"
#include "buzzer.h"

#include "sequence.h"
//#include "uart.h"
//#include "spi.h"

#include "qutyio.h"

uint32_t MASK = 0xE2023CAB;
uint32_t STATE_LFSR = 0x11186267;
uint16_t sequence_length = 1;
uint16_t score = 0;
uint16_t playback_delay = 1000;
uint8_t buzzer_switch = 1;

void init(void)
{
  // for buzzer
  PORTA.DIRSET = PIN1_bm; // DISP LATCH??
  PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // button3?
  _delay_ms(3000); // 3 second delay

  // for timer 
  clock_init();
  pwm_init();

  // for LED

  // for buttons
  //buttons_init();
  //printf("The stopwatch is done");

}

ISR(TCB1_INT_vect) {
  printf("elapsed time is %u \n", elapsed_time); 

  if ((buzzer_switch == 0) && (elapsed_time == 660)) {
    TCA0.SINGLE.PERBUF = 1579; // freq is 2110   // 3.33/frequency
    TCA0.SINGLE.CMP0BUF = 790; // PWM / 2 = 50% duty cycle
    buzzer_switch = 1;
    elapsed_time = 0;
  } else if ((buzzer_switch == 1) && (elapsed_time == 320) ) {
    TCA0.SINGLE.PERBUF = 685; //686
    TCA0.SINGLE.CMP0BUF = 342;  // 343// for pwm 1/4860 -> // control frequency of the buzzer   time
    buzzer_switch = 0;
    elapsed_time = 0;
  }

  // if (true_elapsed == total_ms_value):
  //    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // <-- This but the opposite

  elapsed_time++;
  TCB1.INTFLAGS = TCB_CAPT_bm;

    // when certain criterion met
    // set is-counting to zero
}