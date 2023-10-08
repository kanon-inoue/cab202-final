#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "initialization.h"
#include "buzzer.h"
#include "project.h"

#include "sequence.h"
#include "uart.h"
#include "spi.h"

uint32_t MASK = 0xE2023CAB;
uint32_t STATE_LFSR = 0x11186267;
uint16_t sequence_length = 1;
uint16_t score = 0;
uint16_t playback_delay = 1000;
uint8_t buzzer_switch = 1;
uint16_t elapsed_time = 0;
uint8_t is_counting = 1;

void init()
{
  // for buzzer
  // PORTA.DIRSET = PIN1_bm; // DISP LATCH??
  // printf("disp latch okay \n");
  // PORTA.PIN7CTRL = PORT_PULLUPEN_bm; // button3?
  // printf("printf okay \n");
  // _delay_ms(3000); // 3 second delay


  // for timer 
  clock_init();
  pwm_init();

  TCA0.SINGLE.PERBUF = 1579; // freq is 2110   // 3.33333/frequency
  TCA0.SINGLE.CMP0BUF = 790; // PWM / 2 = 50% duty cycle

  // for LED
  enable_led();

  // for buttons
  buttons_init();
  

}

// for buzzer
ISR(TCB1_INT_vect) {
  printf("elapsed time is %u \n", elapsed_time); 

  if ((buzzer_switch == 0) && (elapsed_time == 660)) {
    TCA0.SINGLE.PERBUF = 1579; // freq is 2110   // 3.33/frequency
    TCA0.SINGLE.CMP0BUF = 790; // PWM / 2 = 50% duty cycle
    buzzer_switch = 1;
    elapsed_time = 0;
    printf("660 is making noise \n");
  } else if ((buzzer_switch == 1) && (elapsed_time == 320) ) {
    TCA0.SINGLE.PERBUF = 685; //686
    TCA0.SINGLE.CMP0BUF = 342;  // 343// for pwm 1/4860 -> // control frequency of the buzzer   time
    buzzer_switch = 0;
    elapsed_time = 0;
    printf("320 is making noise \n");
  }

  // if (true_elapsed == total_ms_value):
  //    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // <-- This but the opposite

  elapsed_time++;
  TCB1.INTFLAGS = TCB_CAPT_bm;

    // when certain criterion met
    // set is-counting to zero
}

// for button
ISR(PORTA_PORT_vect)
{
  spi_init();
  // push s1
  if (VPORTA.INTFLAGS & PIN4_bm) 
  {
    // light the left left line
    spi_write(0b10111110);
    //0b1FAB GCDE 
    VPORTA_INTFLAGS = PIN4_bm;
  } 
  // stop it
  if (VPORTA.INTFLAGS & PIN5_bm) 
  {
    // light the left right line
    spi_write(0b11101011);
    VPORTA_INTFLAGS = PIN5_bm;
  }
  // s3
  if (VPORTA.INTFLAGS & PIN6_bm) 
  {
    // light the right left line
    spi_write(0b00111110);
    VPORTA_INTFLAGS = PIN6_bm;
  }
  // reset it to 0
  if (VPORTA.INTFLAGS & PIN7_bm) 
  {
    // light the right right line
    spi_write(0b01101011);
    VPORTA_INTFLAGS = PIN7_bm;
  }
}