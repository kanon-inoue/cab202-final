#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "initialization.h"
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

uint8_t pb_previous = 0xFF;
uint8_t pb_current = 0xFF;
uint8_t pb_falling_edge = 0;

void init(void) { // TODO
  clock_init(); // for enable timer 
  pwm_init(); // for enable timer
  enable_led(); // for enable LED
  buttons_init(); // for enable buttons
  spi_init(); // for enable LED desplay
}

volatile uint8_t pb_state;

ISR(TCB1_INT_vect) { // for timer
  static uint32_t sequence[200];

  // Periodic 5 ms interrupt
  static uint8_t count0 = 0;
  static uint8_t count1 = 0;
  uint8_t pb_sample = PORTA.IN; // Sample PB state
  uint8_t pb_changed = pb_sample ^ pb_state; // Detect change to PB
  // pb_changed = pb_changed & pb_state;
  // Increment if PB state changed, reset otherwise
  count1 = (count1 ^ count0) & pb_changed;
  count0 = ~count0 & pb_changed;

  // Update PB state immediately on falling edge or if PB high for three samples
  pb_state ^= (count1 & count0) | (pb_changed & pb_sample);
  if (pb_changed) {
    if (pb_state == PIN4_bm) {
      inputs[current_input] = 1;
      current_input += 1;
    } else if (pb_state == PIN5_bm) {
      inputs[current_input] = 2;
      current_input += 1;
    } else if (pb_state == PIN6_bm) {
      inputs[current_input] = 3;
      current_input += 1;
    } else if (pb_state == PIN7_bm) {
      inputs[current_input] = 4;
      current_input += 1;
    }
  }

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
        buzzer_switch = 0;
      }
      if (elapsed_time == playback_delay) {
        if ((inputs[current_input - 1] != sequence[current_input - 1]) && (inputs[current_input - 1] != '\0')) { // when FAIL
          memset(inputs, '\0', sizeof(inputs)); // reset the input array
          current_input = 0;
          state = FAIL;
          elapsed_time = 0;
        }
        if (current_input == 0) {
          spi_write(0x00); // 8 display
        }
        if (current_input == 5) {
          spi_write(0xBB);
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
  TCB1.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
}

ISR(PORTA_PORT_vect) { // for button 
  if (state == RECV_IDENT) {
    if (VPORTA.INTFLAGS & PIN4_bm) { // push s1
      VPORTA_INTFLAGS = PIN4_bm;
      desplay_play_note(1);
      buzzer_switch = 1;
      elapsed_time = 0;
      //inputs[current_input] = 1;
      // current_input += 1;
    } else if (VPORTA.INTFLAGS & PIN5_bm) { // push s2
      VPORTA_INTFLAGS = PIN5_bm;
      desplay_play_note(2);
      buzzer_switch = 1;
      elapsed_time = 0;
      //inputs[current_input] = 2;
      // current_input += 1;
    } else if (VPORTA.INTFLAGS & PIN6_bm) { // s3
      VPORTA_INTFLAGS = PIN6_bm;
      desplay_play_note(3);
      buzzer_switch = 1;
      elapsed_time = 0;
      //inputs[current_input] = 3;
      // current_input += 1;
    } else if (VPORTA.INTFLAGS & PIN7_bm) { // s4
      VPORTA_INTFLAGS = PIN7_bm;
      desplay_play_note(4);
      buzzer_switch = 1;
      elapsed_time = 0;
      //inputs[current_input] = 4;
      // current_input += 1;
    } else {
    VPORTA_INTFLAGS = 0xFF; // Acknowledge interrupt
    }
  } else {
    VPORTA_INTFLAGS = 0xFF; // Acknowledge interrupt
  }
}