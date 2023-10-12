#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>

#include "project.h"


int main(void)
{
  printf("the main starts runnnig! \n");

  init();

  printf("done");

  while (1)
        ; // Loop indefinitely
}
