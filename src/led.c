#include <avr/io.h>
#include <stdio.h>
#include "spi.h"

uint16_t E_high = 9523; // freq is 350 Hz  // 3.33/frequency
uint16_t C = 11337; // freq is 294 Hz  // 3.33/frequency
uint16_t A = 7137; // freq is 467 Hz  // 3.33/frequency
uint16_t E_low = 19045; // freq is 175 Hz  // 3.33/frequency

// a function for playing a tone and lit for each push button
void display_play_note(uint8_t note_number) {
  if (note_number == 1) {
    TCA0.SINGLE.PERBUF = E_high; 
    TCA0.SINGLE.CMP0BUF = E_high/2; // PWM / 2 = 50% duty cycle
    spi_write(0b10111110); // light up the left line 
  } if (note_number == 2) {
    TCA0.SINGLE.PERBUF = C; 
    TCA0.SINGLE.CMP0BUF = C/2; // PWM / 2 = 50% duty cycle
    spi_write(0b11101011); // light up the second left line
  } if (note_number == 3) {
    TCA0.SINGLE.PERBUF = A; 
    TCA0.SINGLE.CMP0BUF = A/2; // PWM / 2 = 50% duty cycle
    spi_write(0b00111110); // light up the second right line
  } if (note_number == 4) {
    TCA0.SINGLE.PERBUF = E_low; 
    TCA0.SINGLE.CMP0BUF = E_low/2; // PWM / 2 = 50% duty cycle
    spi_write(0b01101011); // light up the right line
  }
}

// a function for display score on the LED display
void display_score(uint16_t sequence_length, uint16_t elapsed_time) {
  uint16_t score = sequence_length - 1;
  uint16_t degit; // for the left degit
  uint16_t remind; // for the right degit
  uint16_t simple_score; // for when there are 3 degits
    simple_score = score % 100; // if the score is 123 then 123/100 = 1 ... 23. That meabs the simple score is 23. Ignore the 3rd degit (right the most).
    degit = simple_score / 10; // it is for the middle degit for 3 degits
    remind = simple_score % 10; // it is for the left degit for 3 degits
    if (elapsed_time % 2 == 1) {
      if (degit == 0 && score > 100) spi_write(0b10001000);
      if (degit == 1) spi_write(0b11101011); 
      if (degit == 2) spi_write(0b11000100);
      if (degit == 3) spi_write(0b11000001);
      if (degit == 4) spi_write(0b10100011);
      if (degit == 5) spi_write(0b10010001);
      if (degit == 6) spi_write(0b10010000);
      if (degit == 7) spi_write(0b10001011);
      if (degit == 8) spi_write(0b10000000);
      if (degit == 9) spi_write(0b10000001);
    } else {
      if (remind == 0) spi_write(0b00001000);
      if (remind == 1) spi_write(0b01101011);
      if (remind == 2) spi_write(0b01000100);
      if (remind == 3) spi_write(0b01000001);
      if (remind == 4) spi_write(0b00100011);
      if (remind == 5) spi_write(0b00010001);
      if (remind == 6) spi_write(0b00010000);
      if (remind == 7) spi_write(0b00001011);
      if (remind == 8) spi_write(0b00000000);
      if (remind == 9) spi_write(0b00000001);
    }
}

// a function for g segments display for failling
void fail_display(uint16_t elapsed_time) {
  if (elapsed_time % 2 == 1) spi_write(0b01110111);
  else spi_write(0b11110111); 
}
