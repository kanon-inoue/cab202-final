#include <avr/io.h>
#include <stdio.h>

#include "led.h"
#include "spi.h"

void enable_led(void) {
  PORTA.OUTSET = PIN1_bm; // enable DISP LATCH
  PORTA.DIRSET = PIN1_bm; // the DISP LATCH as output 
}

void display_score(uint16_t sequence_length) {
  uint16_t score = sequence_length - 1;
  uint16_t degit;
  uint16_t remind;
  if (score < 100) {
    degit = score / 10;
    remind = score % 10; 
    if (degit == 1) spi_write(0b11101011);
    if (degit == 2) spi_write(0b11000100);
    if (degit == 3) spi_write(0b11000001);
    if (degit == 4) spi_write(0b10100011);
    if (degit == 5) spi_write(0b10010001);
    if (degit == 6) spi_write(0b10010000);
    if (degit == 7) spi_write(0b10001011);
    if (degit == 8) spi_write(0b10000000);
    if (degit == 9) spi_write(0b10000001);

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
  } else if (score >  99) {
    // todo
  }
}
