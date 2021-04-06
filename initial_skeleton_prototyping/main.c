#include "MK64F12.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "uart.h"
#include "interrupts.h"

int main(int argc, char *argv[])
{
	uart0_init();
	NVIC_init();
	//todo: use default drivers for uart to get interrupts up and running, then create I2C and SPI code, then create basic ui
	return 0;
}
