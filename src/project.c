#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "initialization.h"
#include "buzzer.h"
#include "project.h"

#include "sequence.h"
#include "uart.h"
#include "spi.h"

typedef enum {
PLAY_NEW_NOTE,
PLAY_EXISTING_NOTE,
WAIT_INPUT,
RECV_IDENT,
RECV_PAYLOAD,
FAIL,
DISPLAY_SCORE,
BLANK_DISPLAY
} serialstate;

uint32_t mask = 0xE2023CAB;
uint32_t studentNum = 0x11186267;
uint16_t sequence_length = 1;
uint16_t score = 0;
uint16_t playback_delay = 1000;
uint8_t buzzer_switch = 0;
uint16_t elapsed_time = 0;
uint8_t is_counting = 1;
uint32_t inputs[200];
uint8_t current_input = 0;
uint8_t current_note_to_play = 0;
uint32_t next_step = 0;


void init()
{
  // for timer 
  clock_init();
  pwm_init();

  // for LED
  enable_led();

  // for buttons
  buttons_init();
  spi_init();
}


// for buzzer
ISR(TCB1_INT_vect) {
  static serialstate state = BLANK_DISPLAY;
  static uint32_t sequence[200];

  switch (state) {
    case BLANK_DISPLAY:
      spi_write(0xF0); // clear display 
      if (elapsed_time == playback_delay) {
        state = PLAY_NEW_NOTE;
        elapsed_time = 0;
      }
      break;
    case PLAY_NEW_NOTE:
      if (next_step == 0) {
        next_step = next(mask, &studentNum);
        if (next_step == 1) {
          TCA0.SINGLE.PERBUF = 9523; // freq is 350 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 4761; // PWM / 2 = 50% duty cycle
          spi_write(0b10111110);
        } else if (next_step == 2) {
          TCA0.SINGLE.PERBUF = 11337; // freq is 294 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 5668; // PWM / 2 = 50% duty cycle
          spi_write(0b11101011);
        } else if (next_step == 3) {
          TCA0.SINGLE.PERBUF = 7137; // freq is 467 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 3569; // PWM / 2 = 50% duty cycle
          spi_write(0b00111110);
        } else if (next_step == 4) {
          TCA0.SINGLE.PERBUF = 19045; // freq is 175 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 9523; // PWM / 2 = 50% duty cycle
          spi_write(0b01101011);
        }
        sequence[sequence_length - 1] = next_step;
      }
      if (elapsed_time == playback_delay/2) {
        TCA0.SINGLE.PERBUF = 0;
        TCA0.SINGLE.CMP0BUF = 0;
        spi_write(0xFF); // clear display
      }
      if (elapsed_time == playback_delay) {
        elapsed_time = 0;
        next_step = 0;
        state = RECV_IDENT;
      }
      break;
    case PLAY_EXISTING_NOTE:
      if (current_note_to_play == sequence_length - 1) {
        current_note_to_play = 0;
        state = PLAY_NEW_NOTE;
      } else {
        buzzer_switch = 1;
        elapsed_time = 0;
        if (sequence[current_note_to_play] == 1) {
          TCA0.SINGLE.PERBUF = 9523; // freq is 350 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 4761; // PWM / 2 = 50% duty cycle
          spi_write(0b10111110);
        } else if (sequence[current_note_to_play] == 2) {
          TCA0.SINGLE.PERBUF = 11337; // freq is 294 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 5668; // PWM / 2 = 50% duty cycle
          // spi_write(0b11101011);
          spi_write(0x00);
        } else if (sequence[current_note_to_play] == 3) {
          TCA0.SINGLE.PERBUF = 7137; // freq is 467 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 3569; // PWM / 2 = 50% duty cycle
          spi_write(0b00111110);
        } else if (sequence[current_note_to_play] == 4) {
          TCA0.SINGLE.PERBUF = 19045; // freq is 175 Hz  // 3.33/frequency
          TCA0.SINGLE.CMP0BUF = 9523; // PWM / 2 = 50% duty cycle
          spi_write(0b01101011);
        }
      }
      if ((buzzer_switch == 1) && (elapsed_time == (playback_delay / 2))) {
        TCA0.SINGLE.PERBUF = 0;
        TCA0.SINGLE.CMP0BUF = 0;
        spi_write(0x00); // clear display
        buzzer_switch = 0;
      }
      if (elapsed_time == playback_delay) {
        state = PLAY_NEW_NOTE;
        current_note_to_play += 1;
        elapsed_time = 0;
      }
      break;
    case RECV_IDENT:
      if ((buzzer_switch == 1) && (elapsed_time == (playback_delay / 2))) {
        TCA0.SINGLE.PERBUF = 0;
        TCA0.SINGLE.CMP0BUF = 0;
        spi_write(0xFF); // clear display
        elapsed_time = 0;
        buzzer_switch = 0;
        if ((inputs[current_input - 1] != sequence[current_input - 1]) && (inputs[current_input - 1] != NULL)) {
          memset(inputs, '\0', sizeof(inputs)); // reset the input array
          current_input = 0;
          state = BLANK_DISPLAY;
        }
        if (current_input == sequence_length) {
          sequence_length += 1;
          memset(inputs, '\0', sizeof(inputs)); // reset the input array
          current_input = 0;
          state = PLAY_EXISTING_NOTE;
        }
      }
      break;
  }

  elapsed_time++;
  TCB1.INTFLAGS = TCB_CAPT_bm;

    // when certain criterion met
    // set is-counting to zero
}

// for button
ISR(PORTA_PORT_vect)
{
  // push s1
  if (VPORTA.INTFLAGS & PIN4_bm) 
  {
    // light the left left line
    // E (high) == 467 * 2^(-5/12) == 349.85 ~= 350 Hz -> 9522.86 for perbuf -> INC = 19046  DEC = 4761
    spi_write(0b10111110); //0b1FAB GCDE 
    VPORTA_INTFLAGS = PIN4_bm;
    TCA0.SINGLE.PERBUF = 9523; // freq is 350 Hz  // 3.33/frequency
    TCA0.SINGLE.CMP0BUF = 4761; // PWM / 2 = 50% duty cycle
    buzzer_switch = 1;
    elapsed_time = 0;
    inputs[current_input] = 1;
    current_input += 1;
  } 
  // push s2
  // C# == 467 * 2^(-8/12) == 294.19 ~= 294 Hz -> 11336.73 for perbuf
  if (VPORTA.INTFLAGS & PIN5_bm) 
  {
    // light the left right line
    spi_write(0b11101011);
    VPORTA_INTFLAGS = PIN5_bm;
    TCA0.SINGLE.PERBUF = 11337; // freq is 294 Hz  // 3.33/frequency
    TCA0.SINGLE.CMP0BUF = 5668; // PWM / 2 = 50% duty cycle
    buzzer_switch = 1;
    elapsed_time = 0;
    inputs[current_input] = 2;
    current_input += 1;
  }
  // s3
  // A == 467 Hz
  if (VPORTA.INTFLAGS & PIN6_bm) 
  {
    // light the right left line
    spi_write(0b00111110);
    VPORTA_INTFLAGS = PIN6_bm;
    TCA0.SINGLE.PERBUF = 7137; // freq is 467 Hz  // 3.33/frequency
    TCA0.SINGLE.CMP0BUF = 3569; // PWM / 2 = 50% duty cycle
    buzzer_switch = 1;
    elapsed_time = 0;
    inputs[current_input] = 3;
    current_input += 1;
  }
  // s4
  // E (low) == 467 * 2^(-17/12) == 174.93 ~= 175 Hz
  if (VPORTA.INTFLAGS & PIN7_bm) 
  {
    // light the right right line
    spi_write(0b01101011);
    VPORTA_INTFLAGS = PIN7_bm;
    TCA0.SINGLE.PERBUF = 19045; // freq is 175 Hz  // 3.33/frequency
    TCA0.SINGLE.CMP0BUF = 9523; // PWM / 2 = 50% duty cycle
    buzzer_switch = 1;
    elapsed_time = 0;
    inputs[current_input] = 4;
    current_input += 1;
  }
}