#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <string.h>
#include <util/delay.h>

#include "buzzer.h"
#include "led.h"
#include "sequence.h"
#include "spi.h"

#define MIN_OCTAVE 1
#define MAX_OCTAVE 9

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
volatile enum serialstate state = BLANK_DISPLAY;
uint16_t elapsed_time = 0;

void change_octave(void) {
  TCA0.SINGLE.PERBUF = 60606u >> (octave - 1);
  if (state == RECV_IDENT) { // TODO
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1;
  }
}
void inc_octave(void) {
  if (octave < MAX_OCTAVE) {
    octave ++;
    change_octave();
  }
}
void dec_octave(void) {
  if (octave > MIN_OCTAVE) {
    octave --;
    change_octave();
  }
}

void play_buzzer(void) {
  TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PER >> 1;
  // state = PLAYING;
  buzzer_timer = 0;
}

void stop_buzzer(void) {
  TCA0.SINGLE.CMP0BUF = 0;
  // state = NOTPLAYING
}

int main(void)
{
  cli();
  PORTB.DIRSET = PIN0_bm; // the Buzzer as output --> TCA0WO0
  // Enable pull-up resistors for PBs
  PORTA_PIN4CTRL |= PORT_PULLUPEN_bm; // s1  PORT_PULLPEN_bm
  PORTA_PIN5CTRL |= PORT_PULLUPEN_bm; // s2   0b00001011
  PORTA_PIN6CTRL |= PORT_PULLUPEN_bm; // s3 0b00001011
  PORTA_PIN7CTRL |= PORT_PULLUPEN_bm; // s4 0b00001011

  // TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc; // 3.33 Mhz (prescalar = 1) // TODO
  TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm | TCA_SINGLE_WGMODE_SINGLESLOPE_gc; // turn on buzzer, WO0, single slope 
  // decide period. before turn off -> PER after turn on --> PERBUF
  TCA0.SINGLE.PER = 9259u; // for pitch  // CPU_CLK_SPEED / Disired_freq  // 3.33333333333 Mhz / Frequency (360)
  TCA0.SINGLE.CMP0 = 0; // for volume   // buzzer - see schematic    // connects to waveform output 0, whhich is CMP0
  // TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PER >> 1; // per devide by 2
  TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;  // enable the timer

  // TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Configure TCB0 in periodic interrupt mode // TODO
  TCB0.INTCTRL |= TCB_CAPT_bm; // CAPT interrupt enable
  TCB0.CCMP = 3333; // Set interval for 1ms (3333 clocks @ 3.3 MHz) 
  TCB0.CTRLA |= TCB_ENABLE_bm; // Enable
  
  PORTB.DIRSET = PIN2_bm; // for UART TX

  USART0.CTRLA |= USART_RXCIE_bm;// uart start intrupt
  USART0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm; // recieve enable // on serial communication
  USART0.BAUD = 1389; // 9600 baud

  PORTA.OUTCLR = PIN1_bm; 
  PORTA.DIRSET = PIN1_bm; // the DISP LATCH as output 
  sei();

  // clock_init(); // for enable timer 
  // pwm_init(); // for enable timer and led
  // buttons_init(); // for enable buttons
  spi_init(); // for enable LED desplay

  uint32_t mask = 0xE2023CAB;
  uint32_t student_num = 0x11186267; 
  uint16_t sequence_length = 1; 
  uint16_t playback_delay = 1000; // TODO 1000?
  uint8_t buzzer_switch = 0; // the buzzer switch is off
  uint32_t inputs[200]; // TODO the maximum score is 
  uint8_t current_input = 0;
  uint8_t current_note_to_play = 0;
  uint8_t previous_note;
  uint32_t next_step = 0;
  static uint32_t sequence[200];
  uint8_t button_currently_pressed = 0;
  
  uint8_t pb_previous = dbinput;

  while (1)
  {
    uint8_t input = dbinput;
    uint8_t pressed = (pb_previous ^ dbinput) & pb_previous;
    uint8_t released = (pb_previous ^ dbinput) & dbinput;
    // uint8_t button_currently_pressed = dbinput;
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
        if (pressed & PIN4_bm) { 
          inputs[current_input] = 1;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN4_bm;
          desplay_play_note(1);
          buzzer_switch = 1;
          elapsed_time = 0;
          buzzer_timer = 0;
          button_currently_pressed = 1;
        } else if (released & PIN4_bm && (elapsed_time >= (playback_delay / 2))) {
          TCA0.SINGLE.PERBUF = 0;
          TCA0.SINGLE.CMP0BUF = 0;
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          button_currently_pressed = 0;
          elapsed_time = (playback_delay / 2);
        }
        if (pressed & PIN5_bm) {
          inputs[current_input] = 2;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN5_bm;
          desplay_play_note(2);
          buzzer_switch = 1;
          elapsed_time = 0;
          buzzer_timer = 0;
          button_currently_pressed = 1;
        } else if (released & PIN5_bm && (elapsed_time >= (playback_delay / 2))) {
          TCA0.SINGLE.PERBUF = 0;
          TCA0.SINGLE.CMP0BUF = 0;
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          button_currently_pressed = 0;
          elapsed_time = (playback_delay / 2);
        }
        if (pressed & PIN6_bm) {
          inputs[current_input] = 3;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN6_bm;
          desplay_play_note(3);
          buzzer_switch = 1;
          elapsed_time = 0;
          buzzer_timer = 0;
          button_currently_pressed = 1;
        } else if (released & PIN6_bm && (elapsed_time >= (playback_delay / 2))) {
          TCA0.SINGLE.PERBUF = 0;
          TCA0.SINGLE.CMP0BUF = 0;
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          button_currently_pressed = 0;
          elapsed_time = (playback_delay / 2);
        }
        if (pressed & PIN7_bm) {
          inputs[current_input] = 4;
          current_input += 1;
          //VPORTA_INTFLAGS = PIN7_bm;
          desplay_play_note(4);
          buzzer_switch = 1;
          elapsed_time = 0;
          buzzer_timer = 0;
          button_currently_pressed = 1;
        } else if (released & PIN7_bm && (elapsed_time >= (playback_delay / 2))) {
          TCA0.SINGLE.PERBUF = 0;
          TCA0.SINGLE.CMP0BUF = 0;
          spi_write(0xFF); // clear display
          buzzer_switch = 0;
          elapsed_time = (playback_delay / 2);
        }

        if (released & (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm)) {
          button_currently_pressed = 0;
        }

        if (button_currently_pressed == 0) {
          if ((buzzer_switch == 1) && (elapsed_time == (playback_delay / 2))) {
            TCA0.SINGLE.PERBUF = 0;
            TCA0.SINGLE.CMP0BUF = 0;
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
    // ADC0.RESULT * x + 250
    // 255 * x + 250 = 2000
    // x == 6.8627

    //playback_delay = ADC0.RESULT;
    // TCA0.SINGLE.CMP1BUF = ((uint32_t)9259 * result) >> 8;  
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

  if (buzzer_timer < buzzer_delay) buzzer_timer++;

  elapsed_time++;

  PORTA.OUTTGL = PIN1_bm; // if i wanna toggle i have to write here 
  TCB0.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
}

#define NOT_RECEIVING_DELAY 0xFF
ISR(USART0_RXC_vect) {
  static char rxbuf[5];
  static uint8_t rxpos = NOT_RECEIVING_DELAY;
  char rx = USART0.RXDATAL;
  USART0.TXDATAL = rx;

  if (rxpos == NOT_RECEIVING_DELAY) {
    switch (rx) {
      case '1': //1or  "q": S1 during game play
        desplay_play_note(1);
      break;
      case '2': // 2or "w" S2 during game play
        desplay_play_note(2);
      break;
      case '3': //  3 or e S3 during game play
        desplay_play_note(3);
      break;
      case '4': // 4 or r S4 during game play
        desplay_play_note(4);
      break;
      case ',': // , k Increase frequency of tones
        inc_octave();
      break;
      case '.': // . l Decrease frequency of tones
        dec_octave();
      break;
      case '0': // 0 p Reset frequencies to default and sequence index to 0
      break;
      case '9': // 9 o Load new seed for pseudo-random sequence
      break;
    }
  } else {
    rxbuf[rxpos++] = rx;
    if (rxpos == 4) {
      rxpos = NOT_RECEIVING_DELAY;
      int16_t new_delay;
      rxbuf[4] = '\0';
      if (sscanf(rxbuf, "%x", &new_delay) ==1) {
        buzzer_delay = new_delay;
      }
    }
  }
}

uint16_t get_new_playback_delay(uint16_t adc_result) {
  return (adc_result * 6.8627) + 250;
}