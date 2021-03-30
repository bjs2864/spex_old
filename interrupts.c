#include "interrupts.h"
#include "MK64F12.h"

void NVIC_init(void){
	NVIC_EnableIRQ(UART0_RX_TX_IRQn);
}

void UART0_RX_TX_IRQHandler(void){
	return;
}
