#include <avr/io.h>
#include <stdint.h>

// a function for enable the uart (serial monitor)
// Initialise the UART peripheral to 9600 baud, 8N1
void uart_init(void)
{
    PORTB.DIRSET = PIN2_bm; // Output enable USART0 TXD (PB2)
    USART0.BAUD = 1389;                           // 9600 baud @ 3.3 MHz
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Tx/Rx
}

// TODO a function to get a character on the the serial monitor?
// Blocking write of byte c to UART
char uart_getc(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm)); // Wait for data
    return USART0.RXDATAL;
}

// a function to print an input character on the serial monitor
// Blocking read of byte c from UART
void uart_putc(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm)); // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

// a function to print an input string/array on the serial monitor
void uart_puts(char* string) // pointer is used for literaling over a string
{
    while (*string != '\0') { // if the input is not empty/null
        uart_putc(*string); // ptint each character from the beggining by calling the uart_putc function with an argument
        string++; // goes to the next character
    }
    
    // char example[] = "Hello"
    // {'H', 'e', 'l', 'l', 'o', '\0'}
    // therefore we will know when string has finished, when we reach the null character.
    // so we want to write code to loop through each character until we reach '\0'.
    // and then when we are at each char -> uart_putc to be able to send it.
    
}