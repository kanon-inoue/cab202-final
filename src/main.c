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

enum serialstate {
PLAY_NOTE, // for playing exsisting notes which can be 1 to any
RECV_IDENT, // for waiting for the player pushes a buttum input
FAIL, // for when the player fails
DISPLAY_SCORE, // for desplaying the score on the LED display on the QUTy and serial monitor
BLANK_DISPLAY // for blank the LED display
};

volatile uint16_t buzzer_timer = 0u;
volatile uint16_t buzzer_delay = 2000u;
volatile uint8_t dbinput = 0xFF;
volatile enum serialstate state = PLAY_NOTE;
volatile char input_char = '\0';
uint16_t elapsed_time = 0;
uint16_t is_counting = 0;

int main(void)
{
  cli();
  button_init(); 
  pwm_init();
  timer_init();
  clock_init();
  uart_init();
  potentiometer_init();
  spi_init();
  sei();

  uint16_t sequence_length = 1; 
  uint8_t buzzer_switch = 0; // the buzzer switch is off
  uint32_t inputs[200]; // TODO the maximum score is 
  uint8_t current_input = 0;
  uint8_t current_note_number_to_play = 0;
  uint8_t note;
  uint8_t button_currently_pressed = 0;
  
  uint8_t pb_previous = dbinput;

  #define TX_BUFFER_SIZE 30
  char tx_buffer[TX_BUFFER_SIZE];

  while (1)
  {
    uint32_t playback_delay = get_new_playback_delay(ADC0.RESULT); // for potentiometer
    //snprintf(tx_buffer, TX_BUFFER_SIZE, "My playback_delay: %u\n", playback_delay); // USART0 already initialised
    // uart_puts(tx_buffer);
    uint8_t input = dbinput;
    uint8_t pressed = (pb_previous ^ dbinput) & pb_previous;
    uint8_t released = (pb_previous ^ dbinput) & dbinput;
    pb_previous = input;

    if ((input_char == ',') || (input_char == 'k')) { // increase frequency of tones
      inc_freq();
      input_char = '\0';
    }
    if ((input_char == '.') || (input_char == 'l')) { // decrease frequency of tones
      dec_freq();
      input_char = '\0';
    }
    if ((input_char == '0') || (input_char == 'p')) { // reset frequencies to default and sequence index to 0
      uint8_t tone = inputs[current_input];
      uint32_t pseudo_random_sequence_seed; 
      // resett all frequencies back to the default
      E_high = 9523;
      C = 11337;
      A = 7137; 
      E_low = 19045;
      // reset the pseudo-random sequence seed back to the initial seed // TODO
      // pseudo_random_sequence_seed = initial_seed;
      // reset the sequence length back to one
      sequence_length = 1;
      input_char = '\0';
      buzzer_switch = 0;
      current_input = 0;
      current_note_number_to_play = 0;
      button_currently_pressed = 0;
      elapsed_time = 0;
      memset(inputs, '\0', sizeof(inputs)); // reset the input array
      state = PLAY_NOTE;
    }
    if ((input_char == '9') || (input_char == 'o')) { // load new seed to for pseudo-random sequencean be received through the UART, and their corresponding actions are summarised in Table 3.
      uint32_t newLFSR;
      input_char = '\0';
    }

    switch (state) {
      case BLANK_DISPLAY:
        spi_write(0xFF); // clear desplay
        if (elapsed_time == playback_delay) {
          state = PLAY_NOTE;
          elapsed_time = 0;
          sequence_length = 1;
        }
      break;
      case PLAY_NOTE:
        if (current_note_number_to_play == sequence_length) {
          current_note_number_to_play = 0;
          state = RECV_IDENT;
        } else {
          if (buzzer_switch == 0) {
            buzzer_switch = 1;
            note = get_note_for_index(current_note_number_to_play);
            desplay_play_note(note);
          }
          if ((buzzer_switch == 1) && (elapsed_time == (playback_delay / 2))) {
            stop_buzzer();
            spi_write(0xFF); // clear the LED display
          }
          if (elapsed_time == playback_delay) {
            buzzer_switch = 0;
            current_note_number_to_play += 1;
            elapsed_time = 0;
          }
        }
      break;
      case RECV_IDENT:
        if ((pressed & PIN4_bm) || (input_char == '1') || (input_char == 'q')){ 
          inputs[current_input] = 1;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN4_bm;
          desplay_play_note(1);
          buzzer_switch = 1;
          elapsed_time = 0;
          input_char = '\0';
          button_currently_pressed = 1;
        } else if ((released & PIN4_bm) && (elapsed_time >= (playback_delay / 2))) {
          stop_buzzer();
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          button_currently_pressed = 0;
          elapsed_time = (playback_delay / 2);
        }
        if ((pressed & PIN5_bm) || (input_char == '2') || (input_char == 'w')) {
          inputs[current_input] = 2;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN5_bm;
          desplay_play_note(2);
          buzzer_switch = 1;
          elapsed_time = 0;
          input_char = '\0';
          button_currently_pressed = 1;
        } else if (released & PIN5_bm && (elapsed_time >= (playback_delay / 2))) {
          stop_buzzer();
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          button_currently_pressed = 0;
          elapsed_time = (playback_delay / 2);
        }
        if ((pressed & PIN6_bm) || (input_char == '3') || (input_char == 'e')) {
          inputs[current_input] = 3;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN6_bm;
          desplay_play_note(3);
          buzzer_switch = 1;
          elapsed_time = 0;
          input_char = '\0';
          button_currently_pressed = 1;
        } else if (released & PIN6_bm && (elapsed_time >= (playback_delay / 2))) {
          stop_buzzer();
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          button_currently_pressed = 0;
          elapsed_time = (playback_delay / 2);
        }
        if ((pressed & PIN7_bm) || (input_char == '4') || (input_char == 'r')) {
          inputs[current_input] = 4;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN7_bm;
          desplay_play_note(4);
          buzzer_switch = 1;
          elapsed_time = 0;
          input_char = '\0';
          button_currently_pressed = 1;
        } else if (released & PIN7_bm && (elapsed_time >= (playback_delay / 2))) {
          stop_buzzer();
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          button_currently_pressed = 0;
          elapsed_time = (playback_delay / 2);
        }
        if (buzzer_switch == 1 && elapsed_time >= (playback_delay / 2)) {
          stop_buzzer();
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          elapsed_time = (playback_delay / 2);
          button_currently_pressed = 0;
        }
        if (released & (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm)) {
          button_currently_pressed = 0;
        }

        if (button_currently_pressed == 0) {
          if ((buzzer_switch == 1) && (elapsed_time == (playback_delay / 2))) {
            stop_buzzer();
            spi_write(0xFF); // clear display
            buzzer_switch = 0;
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
      break;
    }
  } 
}

# define DEBOUNCE_BUFFER 8 // 8 miliseconds

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

  //if (buzzer_timer < buzzer_delay) buzzer_timer++;

  elapsed_time++;

  PORTA.OUTTGL = PIN1_bm; // if i wanna toggle i have to write here 
  TCB0.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
}

// #define NOT_RECEIVING_DELAY 0xFF
ISR(USART0_RXC_vect) {
  input_char = USART0.RXDATAL;
}
