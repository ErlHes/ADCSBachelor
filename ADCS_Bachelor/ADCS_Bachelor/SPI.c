/*
 * SPI.c
 *
 * Created: 20.02.2020 11:28:54
 *  Author: Edvard
 */ 
#include <avr/io.h>
#define read 0b10000000
#define write 0b00000000

void spiInit(void){
	DDRB = (1<<DDB5)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1);		// MOSI, SCK, CS_M and CS_AG output
	PORTB = (1<<PORTB4)|(1<<PORTB2)|(1<<PORTB1);		// pullup on MISO and ~SS not active/high
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);				// SPI enable, Master, Clockdiv 16
}

uint8_t spiRead(uint8_t csPin, uint8_t subAddress){
	uint8_t temp = 0;
	PORTB &= ~(1<<csPin);				// start SPI conversation
	SPDR = 0x80 | (subAddress & 0x3f);	// reads from subadress
	while(!(SPSR & (1<<SPIF))){} 		// wait for transfer to complete
	temp = SPDR;
	PORTB = (1<<csPin);					// end transmission
	return temp;
}

void spiWrite(uint8_t csPin, uint8_t subAddress, uint8_t data){
	PORTB &= ~(1<<csPin);				// start SPI conversation
	SPDR = subAddress & 0x3f;			// Sends registry address
	while(!(SPSR & (1<<SPIF))){}		// wait for transfer to complete
	SPDR = data;						// transmit data
	while(!(SPSR & (1<<SPIF))){}		// wait for transfer to complete
	PORTB = (1<<csPin);					// end transmission
}