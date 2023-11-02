#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

uint32_t next(uint32_t MASK, uint32_t *STATE_LFSR);
uint32_t get_note_for_index(uint32_t index);