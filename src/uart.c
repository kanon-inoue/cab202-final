#include <avr/io.h>

#include <stdint.h>

void uart_init(void)
{
    // Output enable USART0 TXD (PB2)
    PORTB.DIRSET = PIN2_bm;

    USART0.BAUD = 1389;                           // 9600 baud @ 3.3 MHz
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Tx/Rx
}

char uart_getc(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
        ; // Wait for data
    return USART0.RXDATAL;
}

void uart_putc(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
        ; // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

/** CODE: Write your code for Ex 9.0 below this line. */
void uart_puts(char* string)
{
    while (*string != '\0') {
        uart_putc(*string);
        string++;
    }
    
    // char example[] = "Hello"
    // {'H', 'e', 'l', 'l', 'o', '\0'}
    // therefore we will know when string has finished,
    // when we reach the null character

    // so we want to write code to loop through each character
    // until we reach '\0'
    // and then when we are at each char -> uart_putc
    // to be able to send it
    
}

/** CODE: Write your code for Ex 9.0 above this line. */
