#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

volatile uint8_t pb_debounced_state = 0xFF;

// ISR(TCB0_INT_vect) { // for timer

//   // Periodic 1 ms interrupt
//   static uint8_t count0 = 0;
//   static uint8_t count1 = 0;
//   uint8_t pb_sample = PORTA.IN; // Sample PB state
//   uint8_t pb_changed = pb_sample ^ pb_debounced_state; // Detect change to PB
//   // pb_changed = pb_changed & pb_state;
//   // Increment if PB state changed, reset otherwise
//   count1 = (count1 ^ count0) & pb_changed;
//   count0 = ~count0 & pb_changed;

//   // Update PB state immediately on falling edge or if PB high for three samples
//   pb_debounced_state ^= (count1 & count0) | (pb_changed & pb_sample);
  
//   TCB0.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
// }

// ISR(PORTA_PORT_vect) { // for button 
//   if (state == RECV_IDENT) {
//     if (VPORTA.INTFLAGS & PIN4_bm) { // push s1
//       VPORTA_INTFLAGS = PIN4_bm;
//       desplay_play_note(1);
//       buzzer_switch = 1;
//       elapsed_time = 0;
//       //inputs[current_input] = 1;
//       // current_input += 1;
//     } else if (VPORTA.INTFLAGS & PIN5_bm) { // push s2
//       VPORTA_INTFLAGS = PIN5_bm;
//       desplay_play_note(2);
//       buzzer_switch = 1;
//       elapsed_time = 0;
//       //inputs[current_input] = 2;
//       // current_input += 1;
//     } else if (VPORTA.INTFLAGS & PIN6_bm) { // s3
//       VPORTA_INTFLAGS = PIN6_bm;
//       desplay_play_note(3);
//       buzzer_switch = 1;
//       elapsed_time = 0;
//       //inputs[current_input] = 3;
//       // current_input += 1;
//     } else if (VPORTA.INTFLAGS & PIN7_bm) { // s4
//       VPORTA_INTFLAGS = PIN7_bm;
//       desplay_play_note(4);
//       buzzer_switch = 1;
//       elapsed_time = 0;
//       //inputs[current_input] = 4;
//       // current_input += 1;
//     } else {
//     VPORTA_INTFLAGS = 0xFF; // Acknowledge interrupt
//     }
//   } else {
//     VPORTA_INTFLAGS = 0xFF; // Acknowledge interrupt
//   }
// }


    // if (pb_changed) {
    // if (pb_state == PIN4_bm) {
    //   inputs[current_input] = 1;
    //   current_input += 1;
    // } else if (pb_state == PIN5_bm) {
    //   inputs[current_input] = 2;
    //   current_input += 1;
    // } else if (pb_state == PIN6_bm) {
    //   inputs[current_input] = 3;
    //   current_input += 1;
    // } else if (pb_state == PIN7_bm) {
    //   inputs[current_input] = 4;
    //   current_input += 1;
    // }