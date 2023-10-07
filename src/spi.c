// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <stdint.h>

// void spi_init() {
//   cli(); // disable interrupts 
//   PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc; // enable spi on PC0-3
//   PORTC.DIRSET = (PIN0_bm | PIN2_bm); // SPI CLK ans SPI MOIS enable as output
//   PORTA.OUTSET = PIN1_bm; 
//   PORTA.DIRSET = PIN1_bm; // Enable Disp Latch as output
  
//   SPI0.CTRLA = SPI_MASTER_bm; // master mode | MSB first | prescalar / 4
//   SPI0.CTRLB = SPI_SSD_bm; // unbuffered | disable client select line
//   SPI0.INTCTRL = SPI_IE_bm; // enable interrupt using IE bit
//   SPI0.CTRLA |= SPI_ENABLE_bm; // enable SPI0 peripheral 
//   sei(); // enable intrrupts 
// }

// void spi_write(uint8_t data) {
//   SPI0.DATA = data; 
// }

// ISR(SPI0_INT_vect)
// {
//   // Note: to create a rising edge, 
//   // our signal needs to go from low to high 
//   PORTA.OUTCLR = PIN1_bm;
//   PORTA.OUTSET = PIN1_bm;
//   SPI0.INTFLAGS = 0b10000000; // datasheet pg336
// }