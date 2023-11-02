#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h> // contains prototypes for functions that read input and write output
#include <stdint.h> // read input
#include <string.h>
#include <util/delay.h>

#include "buzzerTimers.h"
#include "led.h"
#include "sequence.h"
#include "spi.h"
#include "uart.h"
#include "potentiometer.h"
#include "uartplay.h"

#define TX_BUFFER_SIZE 30

char tx_buffer[TX_BUFFER_SIZE];

enum serialstate {
  PLAY_NOTE, // for playing exsisting notes which can be 1 to any
  RECV_INPUT, // for waiting for the player pushes a buttum input
  FAIL, // for when the player fails
  DISPLAY_SCORE, // for displaying the score on the LED display on the QUTy and serial monitor
  BLANK_DISPLAY // for blank the LED display
};

volatile uint8_t dbinput = 0xFF; // Debounced input
volatile enum serialstate state = PLAY_NOTE;
volatile char input_char = '\0';
uint16_t elapsed_time = 0;
uint16_t is_counting = 0;

int main(void)
{
  // Initial setup of QUTy features
  cli();
  button_init(); 
  pwm_init();
  timer_init();
  clock_init();
  uart_init();
  potentiometer_init();
  spi_init();
  sei();

  // Set initial values for variables
  uint16_t sequence_length = 1; 
  uint8_t playing_or_waiting_for_note = 0; // The buzzer is off at the start
  uint32_t inputs[200];
  uint8_t current_input = 0;
  uint8_t current_note_number_to_play = 0;
  uint8_t current_note_to_play;
  uint8_t button_currently_pressed = 0;
  uint8_t pb_previous = dbinput;

  // Main loop
  while (1)
  {
    uint32_t playback_delay = get_new_playback_delay(ADC0.RESULT); // Reads the potentiometer value and sets the playback delay

    // Set input to a debounced value
    uint8_t input = dbinput;
    uint8_t pressed = (pb_previous ^ dbinput) & pb_previous;
    uint8_t released = (pb_previous ^ dbinput) & dbinput;
    pb_previous = input;

    if ((input_char == ',') || (input_char == 'k')) { // Increase frequency of tones
      inc_freq();
      input_char = '\0';
    }
    if ((input_char == '.') || (input_char == 'l')) { // Decrease frequency of tones
      dec_freq();
      input_char = '\0';
    }
    if ((input_char == '0') || (input_char == 'p')) { // Reset frequencies to default and sequence index to 0
      // Reset all frequencies back to the default
      E_high = 9523;
      C = 11337;
      A = 7137; 
      E_low = 19045;

      // Reset the variables back to their defaults
      sequence_length = 1;
      input_char = '\0';
      playing_or_waiting_for_note = 0;
      current_input = 0;
      current_note_number_to_play = 0;
      button_currently_pressed = 0;
      elapsed_time = 0;
      memset(inputs, '\0', sizeof(inputs)); // Reset the input array
      state = PLAY_NOTE;
    }

    switch (state) {
      case PLAY_NOTE:
        if (current_note_number_to_play == sequence_length) { // Reached the end of current sequence
          // Listen for user input
          current_note_number_to_play = 0;
          state = RECV_INPUT;
        } else {
          if (playing_or_waiting_for_note == 0) { // Play next note
            playing_or_waiting_for_note = 1;
            current_note_to_play = get_note_for_index(current_note_number_to_play);
            display_play_note(current_note_to_play);
          }
          if ((playing_or_waiting_for_note == 1) && (elapsed_time == (playback_delay / 2))) { // Stop playing note after delay
            stop_buzzer();
            spi_write(0xFF); // clear the LED display
          }
          if (elapsed_time == playback_delay) { // Move on to next note
            playing_or_waiting_for_note = 0;
            current_note_number_to_play += 1;
            elapsed_time = 0;
          }
        }
      break;
      case RECV_INPUT:
        if (
          (pressed & PIN4_bm) || (input_char == '1') || (input_char == 'q') ||
          (pressed & PIN5_bm) || (input_char == '2') || (input_char == 'w') ||
          (pressed & PIN6_bm) || (input_char == '3') || (input_char == 'e') ||
          (pressed & PIN7_bm) || (input_char == '4') || (input_char == 'r')
        ) {
          if ((pressed & PIN4_bm) || (input_char == '1') || (input_char == 'q')){ // Button 1 pressed
            inputs[current_input] = 1;
            current_input += 1;
            display_play_note(1);
          }
          if ((pressed & PIN5_bm) || (input_char == '2') || (input_char == 'w')) { // Button 2 pressed
            inputs[current_input] = 2;
            current_input += 1;
            display_play_note(2);
          }
          if ((pressed & PIN6_bm) || (input_char == '3') || (input_char == 'e')) { // Button 3 pressed
            inputs[current_input] = 3;
            current_input += 1;
            display_play_note(3);
          }
          if ((pressed & PIN7_bm) || (input_char == '4') || (input_char == 'r')) { // Button 4 pressed
            inputs[current_input] = 4;
            current_input += 1;
            display_play_note(4);
          }
          playing_or_waiting_for_note = 1;
          elapsed_time = 0;
          input_char = '\0';
          button_currently_pressed = 1;
        }
        if (released & (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm)) { // Button manually released
          button_currently_pressed = 0;
        }
        if (released & (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm) && (elapsed_time >= (playback_delay / 2))) { // Button released after holding long
            stop_buzzer();
            spi_write(0xFF); // clear display
            playing_or_waiting_for_note = 0;
            button_currently_pressed = 0;
            elapsed_time = (playback_delay / 2);
        }
        if (playing_or_waiting_for_note == 1 && elapsed_time >= (playback_delay / 2)) {
          stop_buzzer();
          spi_write(0xFF); // clear display
          playing_or_waiting_for_note = 0;
          elapsed_time = (playback_delay / 2);
          button_currently_pressed = 0;
        }

        if (button_currently_pressed == 0) { // Only stop playing the note after the delay if the button is not pressed
          if ((playing_or_waiting_for_note == 1) && (elapsed_time == (playback_delay / 2))) {
            stop_buzzer();
            spi_write(0xFF); // clear display
            playing_or_waiting_for_note = 0;
          }
          if (elapsed_time == playback_delay && current_input > 0) {
            if ((inputs[current_input - 1] != get_note_for_index(current_input - 1)) && (inputs[current_input - 1] != '\0')) { // when FAIL
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
              state = PLAY_NOTE;
            }
          }
        }
      break;
      case FAIL:
        fail_display(elapsed_time); // light up the both g segments
        if (elapsed_time == playback_delay) {
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
      break;
      case BLANK_DISPLAY:
        spi_write(0xFF); // Clear display
        if (elapsed_time == playback_delay) {
          state = PLAY_NOTE;
          elapsed_time = 0;
          sequence_length = 1;
        }
      break;
    }
  } 
}

# define DEBOUNCE_BUFFER 8 // 8 miliseconds

// Timer interrupt
ISR(TCB0_INT_vect) {
  static uint8_t debounce[DEBOUNCE_BUFFER];
  static uint8_t debounce_pos = 0;
  debounce[debounce_pos] = PORTA.IN & (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
  debounce_pos = (debounce_pos + 1) % DEBOUNCE_BUFFER;

  uint8_t all_same = 1;
  for (uint8_t i = 1; i < DEBOUNCE_BUFFER; i++) {
    if (debounce[i] != debounce[0]) {
      all_same = 0;
      break;
    }
  }
  if (all_same) {
    dbinput = debounce[0];
  }

  elapsed_time++;

  TCB0.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
}

// UART input interrupt
ISR(USART0_RXC_vect) {
  input_char = USART0.RXDATAL;
}
