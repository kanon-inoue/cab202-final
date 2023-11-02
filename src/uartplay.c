#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h> // contains prototypes for functions that read input and write output
#include <stdint.h> // read input
#include <string.h>

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