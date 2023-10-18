#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "project.h"
#include "buzzer.h"

// typedef enum {
// PLAY_NEW_NOTE, // for playing a new note after successing
// PLAY_EXISTING_NOTE, // for playing exsisting notes which can be 1 to any
// RECV_IDENT, // for waiting for the player pushes a buttum input
// FAIL, // for when the player fails
// DISPLAY_SCORE, // for desplaying the score on the LED display on the QUTy and serial monitor
// BLANK_DISPLAY // for blank the LED display
// } serialstate;


int main(void)
{
  // PORTB_DIRSET = PIN0_bm; // enable buzzer --> TCA0WO0
  // PORTA.DIRSET = PIN1_bm; // enable led desplay
  // PORTA.PIN4CTRL |= PORT_PULLUPEN_bm; // enable s1
  // PORTA.PIN5CTRL |= PORT_PULLUPEN_bm; // enable s2
  // PORTA.PIN6CTRL |= PORT_PULLUPEN_bm; // enable s3
  // PORTA.PIN7CTRL |= PORT_PULLUPEN_bm; // enable s4

  // TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // turn on buzzer 
  // TCA0.SINGLE.PER = 9523;// decide period. before turn off -> PER after turn on --> PERBUF
  // // TCA0.SINGLE.CMP0 = TCA0.SINGLE.PER >> 1; // per devide by 2
  // TCA0.SINGLE.CMP0 = 0;
  // TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm; // turn on

  init();
  //serialstate state = BLANK_DISPLAY;
  // adc_init();

  while (1)
  {

    
    // ADC0.RESULT * x + 250
    // 255 * x + 250 = 2000
    // x == 6.8627


    //playback_delay = ADC0.RESULT;
    // TCA0.SINGLE.CMP1BUF = ((uint32_t)9259 * result) >> 8;

  }
       // ; // Loop indefinitely
}

uint16_t get_new_playback_delay(uint16_t adc_result) {
  return (adc_result * 6.8627) + 250;
}