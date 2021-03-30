/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:		    Functions fleshed out by Brunon Sztuba (bjs2864@rit.edu)
 *
 */

#include "MK64F12.h"
#include "uart.h"
#define BAUD_RATE 9600      //default baud rate 
#define SYS_CLOCK 20485760 //default system clock (see DEFAULT_SYSTEM_CLOCK  in system_MK64F12.c)
#define UART0TXPCR PORTB_PCR16
#define UART0RXPCR PORTB_PCR17
#define UART3TXPCR PORTB_PCR10
#define UART3RXPCR PORTB_PCR11


/*
This function initializes the UART0 registers, clocks, etc. for UART0 9600 baud rate
*/
void uart0_init(void)
{
	//define variables for baud rate and baud rate fine adjust
	uint16_t ubd, brfa;

	//Enable clock for UART

   SIM_SCGC4 |= SIM_SCGC4_UART0_MASK; //Enables clock on sim scgc 4 for uart0
	 SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;//Enables port b (uart rx and tx)

	//Configure the port control register to alternative 3 (which is UART mode for K64)
	UART0TXPCR=PORT_PCR_MUX(3);
	UART0RXPCR=PORT_PCR_MUX(3);
	/*Configure the UART for establishing serial communication*/

	//Disable transmitter and receiver until proper settings are chosen for the UART module
	UART0_C2&= ~(UART_C2_TE_MASK) ;
  UART0_C2&= ~(UART_C2_RE_MASK) ;

	//Select default transmission/reception settings for serial communication of UART by clearing the control register 1
  UART0_C1 &= 0;

	//UART Baud rate is calculated by: baud rate = UART module clock / (16 ? (SBR[12:0] + BRFD))
	//13 bits of SBR are shared by the 8 bits of UART3_BDL and the lower 5 bits of UART3_BDH 
	//BRFD is dependent on BRFA, refer Table 52-234 in K64 reference manual
	//BRFA is defined by the lower 4 bits of control register, UART0_C4 

	//calculate baud rate settings: ubd = UART module clock/16* baud rate
	ubd = (uint16_t)((SYS_CLOCK)/(BAUD_RATE * 16));  //will have an extra 3 bits 15:13

	//clear SBR bits of BDH
  UART0_BDH &= ~(UART_BDH_SBR_MASK);

	//distribute this ubd in BDH and BDL
	UART0_BDH &= ~(UART_BDH_SBR_MASK);
	UART0_BDH |= UART_BDH_SBR(ubd>>8);
	UART0_BDL = UART_BDL_SBR(ubd);
	

	//BRFD = (1/32)*BRFA 
	//make the baud rate closer to the desired value by using BRFA
	brfa = (((SYS_CLOCK*32)/(BAUD_RATE * 16)) - (ubd * 32));

	//write the value of brfa in UART0_C4
  UART0_C4 &= ~(UART_C4_BRFA_MASK);
  UART0_C4|= UART_C4_BRFA(brfa);
	
	//Enable interrupts on TDRE and RDRF
	UART0_C2|=UART_C2_TIE_MASK;//TDRE
	UART0_C2|=UART_C2_RIE_MASK;//RDRF
	//Enable transmitter and receiver of UART
 	UART0_C2|= UART_C2_TE_MASK ;
  UART0_C2|= UART_C2_RE_MASK ;
	return;
}

/*
This function reads a char from the UART0 data register
*/
uint8_t uart0_getchar(void)
{
	/* Wait until there is space for more data in the receiver buffer*/
	while(!(UART0_S1 & UART_S1_RDRF_MASK )){}//this is polling - what about irq?

	/* Return the 8-bit data from the receiver */
  return UART0_D;
}

/*
This function writes a char to the UART0 data register
*/
void uart0_putchar(char ch)
{
	/* Wait until transmission of previous bit is complete */
  while(!(UART0_S1 & 	UART_S1_TDRE_MASK )){}//this is polling - what about irq?
	
	/* Send the character */
  UART0_D = ch;
}

void uart0_put(char *ptr_str){
	/*use putchar to print string*/
	while(*ptr_str){
		uart0_putchar(*ptr_str++);
	}
}

/*
This function initializes the UART3 registers, clocks, etc. for UART3 9600 baud rate
*/
void uart3_init(void)
{
	//define variables for baud rate and baud rate fine adjust
	uint16_t ubd, brfa;

	//Enable clock for UART
  SIM_SCGC4 |= SIM_SCGC4_UART3_MASK; //Enables clock on sim scgc 4 for uart0
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;//Enables port b (uart rx and tx)

	//Configure the port control register to alternative 3 (which is UART mode for K64)
	UART3TXPCR=PORT_PCR_MUX(3);
	UART3RXPCR=PORT_PCR_MUX(3);
	/*Configure the UART for establishing serial communication*/

	//Disable transmitter and receiver until proper settings are chosen for the UART module
	UART3_C2&= ~(UART_C2_TE_MASK) ;
  UART3_C2&= ~(UART_C2_RE_MASK) ;

	//Select default transmission/reception settings for serial communication of UART by clearing the control register 1
  UART3_C1 &= 0;

	//UART Baud rate is calculated by: baud rate = UART module clock / (16 ? (SBR[12:0] + BRFD))
	//13 bits of SBR are shared by the 8 bits of UART3_BDL and the lower 5 bits of UART3_BDH 
	//BRFD is dependent on BRFA, refer Table 52-234 in K64 reference manual
	//BRFA is defined by the lower 4 bits of control register, UART0_C4 

	//calculate baud rate settings: ubd = UART module clock/16* baud rate
	ubd = (uint16_t)((SYS_CLOCK)/(BAUD_RATE * 16));  //will have an extra 3 bits 15:13

	//clear SBR bits of BDH
  UART3_BDH &= ~(UART_BDH_SBR_MASK);

	//distribute this ubd in BDH and BDL
	UART3_BDH &= ~(UART_BDH_SBR_MASK);
	UART3_BDH |= UART_BDH_SBR(ubd>>8);
	UART3_BDL = UART_BDL_SBR(ubd);
	

	//BRFD = (1/32)*BRFA 
	//make the baud rate closer to the desired value by using BRFA
	brfa = (((SYS_CLOCK*32)/(BAUD_RATE * 16)) - (ubd * 32));

	//write the value of brfa in UART0_C4
	UART3_C4 &= ~(UART_C4_BRFA_MASK);
	UART3_C4|= UART_C4_BRFA(brfa);
	
	//Enable transmitter and receiver of UART
 	UART3_C2|= UART_C2_TE_MASK ;
  UART3_C2|= UART_C2_RE_MASK ;
	return;
}

/*
This function reads a char from the UART3 data register
*/
uint8_t uart3_getchar(void)
{
/* Wait until there is space for more data in the receiver buffer*/
	while(!(UART3_S1 & UART_S1_RDRF_MASK )){}//this is polling - what about irq?

	/* Return the 8-bit data from the receiver */
 return UART3_D;
}

/*
This function writes a char to the UART3 data register
*/
void uart3_putchar(char ch)
{
/* Wait until transmission of previous bit is complete */
  while(!(UART3_S1 & 	UART_S1_TDRE_MASK )){}//this is polling - what about irq?
	
/* Send the character */
  UART3_D = ch;
}

void uart3_put(char *ptr_str){
	/*use putchar to print string*/
	while(*ptr_str){
		uart3_putchar(*ptr_str++);
	}
}
