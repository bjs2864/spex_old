#include "spi.h"
#include "MK64F12.h"

void spi0_init(void){
	SIM_SCGC6|=SIM_SCGC6_SPI0_MASK;//enable clock to SPI0
	SPI0_MCR|=SPI_MCR_MSTR_MASK;//enable master mode
	SPI0_MCR|=SPI_MCR_CONT_SCKE_MASK;//enable continuous serial clock
	SPI0_MCR|=SPI_MCR_MDIS_MASK;//allow software disabling
	return;
}