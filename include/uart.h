#include <stdint.h>

void uart_init(void);
char uart_getc(void);
void uart_putc(const char c);
void uart_puts(const char *const s);

// remember to account for the null terminator
#define TX_BUFFER_SIZE 30
extern char tx_buffer[TX_BUFFER_SIZE]; // transmission buffer