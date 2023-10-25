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

enum serialstate {
PLAY_NEW_NOTE, // for playing a new note after successing
PLAY_EXISTING_NOTE, // for playing exsisting notes which can be 1 to any
RECV_IDENT, // for waiting for the player pushes a buttum input
FAIL, // for when the player fails
DISPLAY_SCORE, // for desplaying the score on the LED display on the QUTy and serial monitor
BLANK_DISPLAY // for blank the LED display
};

volatile uint16_t buzzer_timer = 0u;
volatile uint16_t buzzer_delay = 2000u;
volatile uint8_t octave = 4;
volatile uint8_t dbinput = 0xFF;
volatile enum serialstate state = PLAY_NEW_NOTE;
volatile char input_char = '\0';
uint16_t elapsed_time = 0;
uint16_t is_counting = 0;

uint16_t inc_freq(uint8_t original_tone) {
  if (original_tone == 1) {
    uint32_t new_tone = 9523 * 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 9523;
  } if (original_tone == 2) {
    uint32_t new_tone = 11337 * 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 11337;
  } if (original_tone == 3) {
    uint32_t new_tone = 7137 * 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 7137;
  } if (original_tone == 4) {
    uint32_t new_tone = 19045 * 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 19045;
  }
}
uint16_t dec_freq(uint8_t original_tone) {
  if (original_tone == 1) {
    uint32_t new_tone = 9523 / 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 2381;
  } if (original_tone == 2) {
    uint32_t new_tone = 11337 / 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 2834;
  } if (original_tone == 3) {
    uint32_t new_tone = 7137 / 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 1785;
  } if (original_tone == 4) {
    uint32_t new_tone = 19045 / 2;
    TCA0.SINGLE.PERBUF = new_tone;
    TCA0.SINGLE.CMP0BUF = 4762;
  }
}

uint32_t get_new_playback_delay(uint32_t adc_result) { // we want duty cycle will be 0% and 100% and new result: 0 - 255
  return ((((adc_result+1)*1750UL) >> 8) + 250);
}

int main(void)
{
  cli();
  button_init();
  pwm_init();
  timer_init();
  clock_init();
  uart_init();
  potentiometer_init();
  spi_init(); // for enable LED desplay
  sei();

  uint32_t mask = 0xE2023CAB;
  uint32_t student_num = 0x11186267;
  uint16_t sequence_length = 1; 
  uint8_t buzzer_switch = 0; // the buzzer switch is off
  uint32_t inputs[200]; // TODO the maximum score is 
  uint8_t current_input = 0;
  uint8_t current_note_to_play = 0;
  uint8_t previous_note;
  uint32_t next_step = 0;
  static uint32_t sequence[200];
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
          stop_buzzer();
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
            stop_buzzer();
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
  // static char rxbuf[5];
  // static uint8_t rxpos = NOT_RECEIVING_DELAY;
  input_char = USART0.RXDATAL;

  // if (rxpos == NOT_RECEIVING_DELAY) {
  //   switch (rx) {
  //     case '1': //1or  "q": S1 during game play
  //       desplay_play_note(1);
  //     break;
  //     case '2': // 2or "w" S2 during game play
  //       desplay_play_note(2);
  //     break;
  //     case '3': //  3 or e S3 during game play
  //       desplay_play_note(3);
  //     break;
  //     case '4': // 4 or r S4 during game play
  //       desplay_play_note(4);
  //     break;
  //     case ',': // , k Increase frequency of tones
  //       inc_octave();
  //     break;
  //     case '.': // . l Decrease frequency of tones
  //       dec_octave();
  //     break;
  //     case '0': // 0 p Reset frequencies to default and sequence index to 0

  //     break;
  //     case '9': // 9 o Load new seed for pseudo-random sequence
  //     break;
  //   }
  // } else {
  //   rxbuf[rxpos++] = rx;
  //   if (rxpos == 4) {
  //     rxpos = NOT_RECEIVING_DELAY;
  //     int16_t new_delay;
  //     rxbuf[4] = '\0';
  //     if (sscanf(rxbuf, "%x", &new_delay) ==1) {
  //       buzzer_delay = new_delay;
  //     }
    // }
  // }
}
