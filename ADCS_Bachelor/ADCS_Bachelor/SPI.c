/*	NOTES.
     * The spiRead and Write feature a NOP instruction, this introduces a small delay that can prevent the wait
     * loop form iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"

void spiInit(void){
	DDRB = (1<<DDB5)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1);		// MOSI, SCK, CS_M and CS_AG output
	PORTB = (1<<PORTB4)|(1<<PORTB2)|(1<<PORTB1);		// pullup on MISO. CS_M and CS_AG -> Active/LOW
	PINB = (1<<PINB2)|(1<<PINB1);						// CS_M and CS_AG start high
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);		// SPI enable, Master mode, MSB first, Clockdiv 128'
																			// Clock idle HIGH, Data Captured on Rising edge. SPI mode 3.
}

uint8_t spiRead(uint8_t csPin, uint8_t subAddress){
//	uint8_t temp = 0;
	PINB &= ~(1<<csPin);				// start SPI conversation
	_delay_ms(1);
	SPDR = 0x80 | (subAddress & 0x3f);	// Set address.
	asm volatile("nop");				// See NOTES at the top of the page.
	while (!(SPSR & _BV(SPIF)));		// wait for transfer to complete
//	temp = SPDR;
//	return temp;
	PINB |= (1<<csPin);					// end transmission
	return SPDR;
}

void spiWrite(uint8_t csPin, uint8_t subAddress, uint8_t data){
	PINB &= ~(1<<csPin);				// start SPI transmission
	SPDR = (subAddress & 0x3F);			// Transfer address.
	while(!(SPSR & (1<<SPIF)));			// wait for transfer to complete
	SPDR = data;						// transmit data
	while(!(SPSR & (1<<SPIF)));			// wait for transfer to complete
	PINB |= (1<<csPin);					// end transmission
}

void WhoAmICheck(void){
	uint8_t testM = 0x00;
	uint8_t testXG = 0x00;
	uint16_t whoAmICombined = 0x0000;
	testXG = spiRead(PIN_XG, WHO_AM_I_XG);
	testM = spiRead(PIN_M, WHO_AM_I_M);
	whoAmICombined = (testXG << 8) | testM;
	while(whoAmICombined != ((WHO_AM_I_AG_RSP << 8) | WHO_AM_I_M_RSP)){
		testM = spiRead(PIN_M, WHO_AM_I_M);
		testXG = spiRead(PIN_XG, WHO_AM_I_XG);
		whoAmICombined = (testXG << 8) | (testM);
		printString("\r\n WHO_AM_I_CHECK: ");
		printWord(whoAmICombined);
		printString("");
		_delay_ms(100);
	}
}