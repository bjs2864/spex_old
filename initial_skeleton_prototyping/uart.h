#ifndef UART_H
#define UART_H

void uart0_init(void);
uint8_t uart0_getchar(void);
void uart0_putchar(char ch);;
void uart0_put(char *ptr_str);
void UART0_RX_TX_IRQHandler(void);
#endif
