/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:		    Functions fleshed out by Brunon Sztuba (bjs2864@rit.edu)
 *
 */

#include "MK64F12.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define BAUD_RATE 9600      //default baud rate 
#define SYS_CLOCK 20485760 //default system clock (see DEFAULT_SYSTEM_CLOCK  in system_MK64F12.c)
#define UART0TXPCR PORTB_PCR16
#define UART0RXPCR PORTB_PCR17

char send_char_buffer[256];
char receive_char_buffer[256];
int send_front=0;
int send_back=0;
int receive_front=0;
int receive_back=0;


/*
This function initializes the UART0 registers, clocks, etc. for UART0 9600 baud rate
*/
void uart0_init(void)
{
	memset(send_char_buffer, 0x00, 256*sizeof(int));
	memset(receive_char_buffer, 0x00, 256*sizeof(int));
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
	//13 bits of SBR are shared by the 8 bits of UART0_BDL and the lower 5 bits of UART0_BDH 
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
  if(receive_char_buffer[receive_front]!=0x00){//if there is stuff in receive buffer
		char desired=receive_char_buffer[receive_front];//return it and clear from buffer
		receive_char_buffer[receive_front]=0x00;
		receive_front=(receive_front+1)%256;
		return desired;
	}
	else{
		return 0x00;//nothin in receive buffer
	}
}

/*
This function writes a char to the UART0 data register
*/
void uart0_putchar(char ch)
{
	send_char_buffer[send_back]=ch;
	send_back=(send_back+1) % 256;
}

void uart0_put(char *ptr_str){
	/*use putchar to print string*/
	while(*ptr_str){
		send_char_buffer[send_back]=*ptr_str;
		send_back=(send_back+1) % 256;
		ptr_str++;
	}
}

void UART0_RX_TX_IRQHandler(void){
	if((UART0_S1 & 	UART_S1_TDRE_MASK )&& send_char_buffer[send_front]!=0x00){//if still stuff in buffer to send
		UART0_D=send_char_buffer[send_front];//send next thing
		send_char_buffer[send_front]=0x00;//clear it from buffer
		send_front=(send_front+1) % 256;//increment front of buffer
	}
	if(UART0_S1 & UART_S1_RDRF_MASK ){
		receive_char_buffer[receive_back]=UART0_D;
		receive_back=(receive_back+1)%256;
	}
	return;
}
