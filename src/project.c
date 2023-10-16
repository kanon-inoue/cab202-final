#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "initialization.h"
#include "project.h"
#include "led.h"
#include "buzzer.h"
#include "sequence.h"
#include "uart.h"
#include "spi.h"

typedef enum {
PLAY_NEW_NOTE, // for playing a new note after successing
PLAY_EXISTING_NOTE, // for playing exsisting notes which can be 1 to any
RECV_IDENT, // for waiting for the player pushes a buttum input
FAIL, // for when the player fails
DISPLAY_SCORE, // for desplaying the score on the LED display on the QUTy and serial monitor
BLANK_DISPLAY // for blank the LED display
} serialstate;

uint32_t mask = 0xE2023CAB;
uint32_t student_num = 0x11186267; 
uint16_t sequence_length = 1; 
uint16_t playback_delay = 1000; // TODO
uint8_t buzzer_switch = 0; // the buzzer switch is off
uint16_t elapsed_time = 0;
uint32_t inputs[200]; // TODO the maximum score is 
uint8_t current_input = 0;
uint8_t current_note_to_play = 0;
uint8_t previous_note;
uint32_t next_step = 0;
serialstate state = BLANK_DISPLAY;

void init(void) { // TODO
  clock_init(); // for enable timer 
  pwm_init(); // for enable timer
  enable_led(); // for enable LED
  buttons_init(); // for enable buttons
  spi_init(); // for enable LED desplay
}

ISR(TCB1_INT_vect) { // for timer
  static uint32_t sequence[200];

  switch (state) {
    case BLANK_DISPLAY:
      spi_write(0xFF); // clear desplay
      if (elapsed_time == playback_delay) {
        state = PLAY_NEW_NOTE;
        elapsed_time = 0;
        sequence_length = 1;
        memset(sequence, '\0', sizeof(sequence));
      }
      break;
    case PLAY_NEW_NOTE:
      if (next_step == 0) {
        next_step = next(mask, &student_num);
        desplay_play_note(next_step); // if the next step = 1, then it playes s1's note
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
        if (buzzer_switch == 0) {
          buzzer_switch = 1;
          previous_note = sequence[current_note_to_play];
          desplay_play_note(previous_note); // play the note which was already played and passed 
        }
        if ((buzzer_switch == 1) && (elapsed_time == (playback_delay / 2))) {
          TCA0.SINGLE.PERBUF = 0;
          TCA0.SINGLE.CMP0BUF = 0;
          spi_write(0xFF); // clear the LED display
        }
        if (elapsed_time == playback_delay) {
          buzzer_switch = 0;
          current_note_to_play += 1;
          elapsed_time = 0;
        }
      }
      break;
    case RECV_IDENT:
      if ((buzzer_switch == 1) && (elapsed_time == (playback_delay / 2))) {
        TCA0.SINGLE.PERBUF = 0;
        TCA0.SINGLE.CMP0BUF = 0;
        spi_write(0xFF); // clear display
        elapsed_time = 0;
        buzzer_switch = 0;
      }
      if (elapsed_time == playback_delay) {
        if ((inputs[current_input - 1] != sequence[current_input - 1]) && (inputs[current_input - 1] != '\0')) { // when FAIL
          memset(inputs, '\0', sizeof(inputs)); // reset the input array
          current_input = 0;
          state = FAIL;
          elapsed_time = 0;
        }
        if (current_input == sequence_length) { // when SUCCESS
          sequence_length += 1;
          memset(inputs, '\0', sizeof(inputs)); // reset the input array
          current_input = 0;
          elapsed_time = 0;
          state = PLAY_EXISTING_NOTE;
        }
      }
      break;
    case FAIL: 
      fail_desplay(elapsed_time); // light up the both g segments
      if(elapsed_time == playback_delay) {
        elapsed_time = 0;
        state = DISPLAY_SCORE;
      }
      break;
    case DISPLAY_SCORE:
      display_score(sequence_length, elapsed_time);
      if (elapsed_time == playback_delay) {
        elapsed_time = 0;
        state = BLANK_DISPLAY;
      }
  }
  elapsed_time++;
  TCB1.INTFLAGS = TCB_CAPT_bm;
}

ISR(PORTA_PORT_vect) { // for button 
  if (state == RECV_IDENT) {
    if (VPORTA.INTFLAGS & PIN4_bm) { // push s1
      spi_write(0b10111110); //0b1FAB GCDE // light the left left line
      VPORTA_INTFLAGS = PIN4_bm;
      // E (high) == 467 * 2^(-5/12) == 349.85 ~= 350 Hz -> 9522.86 for perbuf -> INC = 19046  DEC = 4761
      TCA0.SINGLE.PERBUF = 9523; // freq is 350 Hz  // 3.33/frequency
      TCA0.SINGLE.CMP0BUF = 4761; // PWM / 2 = 50% duty cycle
      buzzer_switch = 1;
      elapsed_time = 0;
      inputs[current_input] = 1;
      current_input += 1;
    } 
    if (VPORTA.INTFLAGS & PIN5_bm) { // push s2
      spi_write(0b11101011); // light the left right line
      VPORTA_INTFLAGS = PIN5_bm;
      // C# == 467 * 2^(-8/12) == 294.19 ~= 294 Hz -> 11336.73 for perbuf
      TCA0.SINGLE.PERBUF = 11337; // freq is 294 Hz  // 3.33/frequency
      TCA0.SINGLE.CMP0BUF = 5668; // PWM / 2 = 50% duty cycle
      buzzer_switch = 1;
      elapsed_time = 0;
      inputs[current_input] = 2;
      current_input += 1;
    }
    if (VPORTA.INTFLAGS & PIN6_bm) { // s3
      spi_write(0b00111110); // light the right left line
      VPORTA_INTFLAGS = PIN6_bm;
      TCA0.SINGLE.PERBUF = 7137; // freq is 467 Hz  // 3.33/frequency
      TCA0.SINGLE.CMP0BUF = 3569; // PWM / 2 = 50% duty cycle
      buzzer_switch = 1;
      elapsed_time = 0;
      inputs[current_input] = 3;
      current_input += 1;
    }
    if (VPORTA.INTFLAGS & PIN7_bm) { // s4
      spi_write(0b01101011);  // light the right right line
      VPORTA_INTFLAGS = PIN7_bm;
      // E (low) == 467 * 2^(-17/12) == 174.93 ~= 175 Hz
      TCA0.SINGLE.PERBUF = 19045; // freq is 175 Hz  // 3.33/frequency
      TCA0.SINGLE.CMP0BUF = 9523; // PWM / 2 = 50% duty cycle
      buzzer_switch = 1;
      elapsed_time = 0;
      inputs[current_input] = 4;
      current_input += 1;
    }
  }
}