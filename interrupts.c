#include "interrupts.h"
#include "MK64F12.h"
#include "uart.h"
#include "spi.h"

void NVIC_init(void){
	NVIC_EnableIRQ(UART0_RX_TX_IRQn);
	NVIC_EnableIRQ(SPI0_IRQn);
}
