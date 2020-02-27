#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"

void spiInit(void){
	DDRB = (1<<DDB5)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0);			// MOSI, SCK, CS_M and CS_AG output ||DDB0 is for testing
	PORTB = (1<<PORTB2)|(1<<PORTB1)|(1<<PORTB0);						// CS_M and CS_AG start HIGH || PORTB0 is for testing
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);	// * SPI enable, Master mode, MSB first, Clockdiv 128
	// * Clock idle HIGH, Data Captured on Rising edge. SPI mode 3.
}


uint8_t SPIreadByte(uint8_t csPin, uint8_t subAddress)
{
	uint8_t temp;
	// Use the multiple read function to read 1 byte.
	// Value is returned to `temp`.
	SPIreadBytes(csPin, subAddress, &temp, 1);
	return temp;
}

uint8_t SPIreadBytes(uint8_t csPin, uint8_t subAddress,
uint8_t * dest, uint8_t count)
{
	// To indicate a read, set bit 0 (msb) of first byte to 1
	uint8_t rAddress = 0x80 | (subAddress & 0x3F);
	// Mag SPI port is different. If we're reading multiple bytes,
	// set bit 1 to 1. The remaining six bytes are the address to be read
	if ((csPin == PIN_M) && count > 1)
	rAddress |= 0x40;
	
	PORTB &= ~(1<<csPin);	// Initiate communication
	spiTransfer(rAddress);
	for (int i=0; i<count; i++)
	{
		dest[i] = spiTransfer(0x00); // Read into destination array
	}
	PORTB |= (1<<csPin); // Close communication
	
	return count;
}

uint8_t spiTransfer(uint8_t data) {
    SPDR = data;
    /*
     * The following NOP introduces a small delay that can prevent the wait
     * loop form iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
    asm volatile("nop");
    while (!(SPSR & _BV(SPIF))) ; // wait
    return SPDR;
  }
  
  void WhoAmICheck(void){
	  uint8_t testM = 0x00;
	  uint8_t testXG = 0x00;
	  uint16_t whoAmICombined = 0x0000;
	  //spiWrite(PIN_M, CTRL_REG3_M, 0b00000111);		//Needed to read from the Magnetometer registers.
	  testXG = SPIreadByte(PIN_XG, WHO_AM_I_XG);
	  testM = SPIreadByte(PIN_M, WHO_AM_I_M);
	  whoAmICombined = (testXG << 8) | testM;
	  printString("\r\n WHO_AM_I_CHECK (correct result is 26685: ");
	  printWord(whoAmICombined);
	  while(whoAmICombined != ((WHO_AM_I_AG_RSP << 8) | WHO_AM_I_M_RSP)){
		  testM = SPIreadByte(PIN_M, WHO_AM_I_M);
		  testXG = SPIreadByte(PIN_XG, WHO_AM_I_XG);
		  whoAmICombined = (testXG << 8) | (testM);
		  printString("\r\n WHO_AM_I_CHECK (correct result is 26685: ");
		  printWord(whoAmICombined);
		  printString("");
		  _delay_ms(100);
	  }
  }
  
  void SPIwriteByte(uint8_t csPin, uint8_t subAddress, uint8_t data)
  {
	  PORTB &= ~(1<<csPin); // Initiate communication
	  
	  // If write, bit 0 (MSB) should be 0
	  // If single write, bit 1 should be 0
	  spiTransfer(subAddress & 0x3F); // Send Address
	  spiTransfer(data); // Send data
	  
	  PORTB |= (1<<csPin); // Close communication
  }