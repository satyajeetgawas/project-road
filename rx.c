//*******************CODE FOR RECEIVER OF DATA BEGINS*******************
/*
 This program receives message serially and displays it on LCD
 */
#include <stdio.h>
#include <stdlib.h>
#include <pic16f886.h>
#include <htc.h>
#define synbyte1 'A'
#define synbyte2 'A'
#define DATA PORTB //Data PORT
#define RS PORTCbits.RC1 // RS signal for LCD
#define RW PORTCbits.RC2 // R/W signal for LCD
#define E PORTCbits.RC3 // E signal for LCD
#define TRUE 1
unsigned char x[80]; //global variable
unsigned char length = 0; //global variable

/*Configuration bits*/
__CONFIG(FOSC_HS & WDTE_ON & PWRTE_OFF & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_ON & IESO_ON & FCMEN_ON & LVP_OFF);
__CONFIG(BOR4V_BOR40V & WRT_OFF);

/*subroutines declaration*/
void inituart(void); //initialise UART
unsigned char receive(void); //receive character
void Init_LCD(void); //Initialize LCD dispaly
void cmd(unsigned char); // 8-bit control word for LCD
void data(unsigned char); // 8-bit text data for LCD
void display(void); //display the message
void clear(void); //clear LCD screen
void delay(unsigned int); // delay

/*main program*/
int main() {
	unsigned char syn_byte1 = 0, syn_byte2 = 0;
	unsigned char i, sum;
	TRISB = 0x00; //PORTB as output
	inituart(); //initialise UART
	while (TRUE) {
		clear();
		syn_byte1 = receive(); //get the synbyte1
		if (syn_byte1 != synbyte1) {
			continue; //goto while if mismatch
		}
		syn_byte2 = receive(); //get the synbyte2
		if (syn_byte2 != synbyte2) {
			continue; //goto while if mismatch
		}
		length = receive(); //receive LENGTH number of byte
		sum = 0;
		for (i = 0; i < length; i++) //calculate sum
				{
			x[i] = receive();
			sum = sum + x[i];
		}
		if (sum == 0) //if data not corrupt display message
				{
			display(); //display message
			for (i = 0; i < 10; i++) {
				delay(60000);
			}
			clear();
		} else //if data corrupt clear display
		{
			Init_LCD();
			delay(2000);
		}
	}
	return (EXIT_SUCCESS);
}
/*initialise UART*/
void inituart(void) {
	TRISC7 = 1; //RX Pin
	SPBRG = 103; //BAUD rate of 2400bps
	BRGH = 0; //normal baudrate
	SYNC = 0; //asynchronous
	SPEN = 1; //enable serial port pins
	CREN = 1; //enable reception
	SREN = 0; //no effect
	RCIE = 1; //enable rx interrupts
	RX9 = 0; //8-bit reception
}
/*Gets a character from the serial port*/
unsigned char receive(void) {
	if (OERR) //if over run error, then reset the receiver
	{
		CREN = 0;
		CREN = 1;
	}
	while (!RCIF)
		; //wait for transmission to receive
	return RCREG; //return the recieved byte
}
/*delay subroutine*/
void delay(unsigned int n) {
	unsigned int j;
	for (j = 0; j < n; j++)
		;
}
/*display message*/
void display() {
	unsigned char i;
	TRISB = 0x00; //PORTB as output
	TRISC = 0x80; // set PORTC bit(0:6) as output connected LCD signal
	Init_LCD(); // Init LCD 8bit interface multiple lines
	for (i = 0; i < (length - 1); i++) // output message
			{
		data(x[i]);
	}
}/* LCD display initialization */
void Init_LCD() {
	cmd(0x3C); // Function Set - 8-bit, 2 lines, 5X7
	cmd(0b00001100); // Display on, cursor on
	cmd(0b00000110); // Entry mode - inc addr, no shift
	cmd(0b00000001); // Clear display & home position
	cmd(0b00000010);
}
/* Write control word to LCD */
void cmd(unsigned char d) {
	RW = 0; //write mode
	RS = 0; // Data as command
	DATA = d; //data on PORTB
	E = 1; //Enable HIGH
	delay(50);
	E = 0; // Enable LOW
	delay(1000);
}
/* Write text data to LCD */
void data(unsigned char d) {
	RW = 0; //write mode
	RS = 1; //data as character
	DATA = d; //data on PORTB
	E = 1; //Enable HIGH
	delay(50);
	E = 0; // Enable LOW
	delay(1000);
}
/*clear the display*/
void clear() {
	cmd(0x01);
	delay(10000);
	cmd(0x08);
	delay(10000);
}
