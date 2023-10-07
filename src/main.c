#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "qutyio.h"
#include "project.h"


int main(void)
{
  // display_init();  // Configures pins PA1, PC0, and PC2, to drive the 7-segment display
  // display_on(); // Drives DISP EN net HIGH
  // serial_init();

  printf("the main starts runnnig! \n");

  init();

  printf("done");

  while (1)
        ; // Loop indefinitely
}
