#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

void display_play_note(uint8_t note_number);
void display_score(uint16_t sequence_length, uint16_t elapsed_time);
void fail_display(uint16_t elapsed_time);

extern uint16_t E_high;
extern uint16_t C;
extern uint16_t A;
extern uint16_t E_low;