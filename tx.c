//*******************CODE FOR TRANSMISSION OF DATA BEGINS*******************
/*
 This program transmits the message via TX pin i.e PORTC.6
 */
#include <stdio.h>
#include <stdlib.h>
#include<pic16f886.h>
#include<htc.h>
#define synbyte1 'A'
#define synbyte2 'A'
#define length 35
#define mssg "SPEED BREAKER AHEAD GO SLOW!!"
#define TRUE 1
//Configuration BITS
__CONFIG(FOSC_HS & WDTE_ON & PWRTE_OFF & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_ON & IESO_ON & FCMEN_ON & LVP_OFF);
__CONFIG(BOR4V_BOR40V & WRT_OFF);
//UART initialization subroutine
void inituart(void) {
	TRISC6 = 0; //TX Pin outpin
	SPBRG = 103; //BAUD rate of 2400bps
	BRGH = 0; //normal baudrate
	SPEN = 1; //enable serial port pins
	TXEN = 1; //Enable Trasmission
	SYNC = 0; //asynchronous
	TX9 = 0; //8 bits transmission
}
//Transmit Subroutine
void transmit(unsigned char text) {
	while (!TXIF)
		; //wait for previous transmission to finish
	TXREG = text; //Transmit Character
}
//delay subroutine
void delay(unsigned int n) {
	unsigned int j;
	for (j = 0; j < n; j++)
		;
}
//main program
int main() {
	unsigned char i, sum, chksum;
	unsigned char x[length];
	x[length] = mssg; //message to be transmitted
	TRISB = 0X00; //PortB as output port
	inituart(); //initialise UART
	sum = 0;
	for (i = 0; i < length - 1; i++) //calculation of checksum
			{
		sum = sum + x[i];
	}
	chksum = 0xFF - sum + 1; //checksum generated
	x[length - 1] = chksum; //append checksum to mssg
	while (TRUE) {
		PORTB = 0X80; //status indicator
		delay(60000);
		transmit(synbyte1); //trasmit synbyte 1
		transmit(synbyte2); //transmit synbyte 2
		transmit(length); //transmit message length
		for (i = 0; i < length; i++) {
			transmit(x[i]); //transmit message
		}
		PORTB = 0X00;
		delay(60000);
	}
	return (EXIT_SUCCESS);
} //*******************CODE FOR TRANSMISSION OF DATA ENDS*******************
