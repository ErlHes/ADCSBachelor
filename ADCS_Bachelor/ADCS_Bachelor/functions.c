#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "header.h"
#include "registers.h"

	// USART

void usart_init( uint16_t ubrr) {
	// Set baud rate
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 1stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

}

void usart_putchar(char data) {
	// Wait for empty transmit buffer
	while ( !(UCSR0A & (_BV(UDRE0))) );
	// Start transmission
	UDR0 = data;
}

char usart_getchar(void) {
	// Wait for incoming data
	while ( !(UCSR0A & (_BV(RXC0))) );
	// Return the data
	return UDR0;
}

void usart_pstr(char *s) {
	// loop through entire string
	while (*s) {
		usart_putchar(*s);
		s++;
	}
}

unsigned char kbhit(void) {
	//return nonzero if char waiting  polled version
	unsigned char b;
	b=0;
	if(UCSR0A & (1<<RXC0)) b=1;
	return b;
}

//*********** required for printf *********//
int usart_putchar_printf(char var, FILE *stream) {
	if (var == '\n') usart_putchar('\r');
	usart_putchar(var);
	return 0;
	
}


	// Timer

void timerInit(void){
	TCCR1A = 0x00;					// We don't need to set any bits, we will use normal mode.
	TCCR1B = (1<<CS11)|(1<<CS10);	// Clock Divide 64 on pre-scaler
}
	

	// SPI
 
 void spiInit(void){
	 DDRB = (1<<DDB5)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0);					// MOSI, SCK, CS_M and CS_AG output ||DDB0 is for testing
	 PORTB = (1<<PORTB2)|(1<<PORTB1)|(1<<PORTB0);								// CS_M and CS_AG start HIGH || PORTB0 is for testing
	 SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0);			// * SPI enable, Master mode, MSB first, Clockdiv 128
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

uint8_t SPIreadBytes(uint8_t csPin, uint8_t subAddress, uint8_t * dest, uint8_t count)
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
   
void SPIwriteByte(uint8_t csPin, uint8_t subAddress, uint8_t data)
   {
	   PORTB &= ~(1<<csPin); // Initiate communication
	   
	   // If write, bit 0 (MSB) should be 0
	   // If single write, bit 1 should be 0
	   spiTransfer(subAddress & 0x3F); // Send Address
	   spiTransfer(data); // Send data
	   
	   PORTB |= (1<<csPin); // Close communication
   }


	// General
 
uint8_t getFIFOSamples(void){
	return (SPIreadByte(PIN_XG, FIFO_SRC) & 0x3F);
}

void setFIFO(uint8_t fifoMode, uint8_t fifoThs){
	// Limit threshold - 0x1F (31) is the maximum. If more than that was asked
	// limit it to the maximum.
	uint8_t threshold = fifoThs <= 0x1F ? fifoThs : 0x1F;
	SPIwriteByte(PIN_XG, FIFO_CTRL, ((fifoMode & 0x7) << 5) | (threshold & 0x1F));
}

void enableFIFO(uint8_t enable){
	uint8_t temp = SPIreadByte(PIN_XG, CTRL_REG9);
	if (enable) temp |= (1<<1);
	else temp &= ~(1<<1);
	SPIwriteByte(PIN_XG, CTRL_REG9, temp);
}

uint8_t getInactivity(void){
	uint8_t temp = SPIreadByte(PIN_XG, STATUS_REG_0);
	temp &= (0x10);
	return temp;
}

void configInactivity(uint8_t duration, uint8_t threshold, uint8_t sleepOn){
	uint8_t temp = 0;
	
	temp = threshold & 0x7F;
	if (sleepOn) temp |= (1<<7);
	SPIwriteByte(PIN_XG, ACT_THS, temp);
	
	SPIwriteByte(PIN_XG, ACT_DUR, duration);
}

void configInt(uint8_t interrupt_select, uint8_t generator, uint8_t activeLow, uint8_t pushPull){
	// Write to INT1_CTRL or INT2_CTRL. [interrupt] should already be one of
	// those two values.
	// [generator] should be an OR'd list of values from the interrupt_generators enum
	SPIwriteByte(PIN_XG, interrupt_select, generator);
	
	// Configure CTRL_REG8
	uint8_t temp;
	temp = SPIreadByte(PIN_XG, CTRL_REG8);
	
	if (activeLow) temp |= (1<<5);
	else temp &= ~(1<<5);
	
	if (pushPull) temp &= ~(1<<4);
	else temp |= (1<<4);
	
	SPIwriteByte(PIN_XG, CTRL_REG8, temp);
}

void WhoAmICheck(void){
	  uint8_t testM = 0x00;
	  uint8_t testXG = 0x00;
	  uint16_t whoAmICombined = 0x0000;
	  //spiWrite(PIN_M, CTRL_REG3_M, 0b00000111);		//Needed to read from the Magnetometer registers.
	  testXG = SPIreadByte(PIN_XG, WHO_AM_I_XG);
	  testM = SPIreadByte(PIN_M, WHO_AM_I_M);
	  printf("Conducting WHO_AM_I check, please wait...\n");
	  whoAmICombined = (testXG << 8) | testM;
	  printf("WHO_AM_I reads 0x%X, expected 0x683D\n", whoAmICombined);
	  if(whoAmICombined != ((WHO_AM_I_AG_RSP << 8) | WHO_AM_I_M_RSP)){
		  printf("test failed. \n");
		  printf("double-check wiring and retry, program will not run as long as the check fails.\n");
		  while(1);
	  }
  }


	// Gyroscope

void sleepGyro(uint8_t enable){
	uint8_t temp = SPIreadByte(PIN_XG, CTRL_REG9);
	if (enable) temp |= (1<<6);
	else temp &= ~(1<<6);
	SPIwriteByte(PIN_XG, CTRL_REG9, temp);
}

void readGyro(void){
	gx = (SPIreadByte(PIN_XG, OUT_X_H_G) << 8 | SPIreadByte(PIN_XG, OUT_X_L_G));
	gy = (SPIreadByte(PIN_XG, OUT_Y_H_G) << 8 | SPIreadByte(PIN_XG, OUT_Y_L_G));
	gz = (SPIreadByte(PIN_XG, OUT_Z_H_G) << 8 | SPIreadByte(PIN_XG, OUT_Z_L_G));
}

uint8_t getGyroIntSrc(void){
	uint8_t intSrc = SPIreadByte(PIN_XG, INT_GEN_SRC_G);
	
	// Check if the IA_G (interrupt active) bit is set
	if (intSrc & (1<<6))
	{
		return (intSrc & 0x3F);
	}
	
	return 0;
}

void configGyroInt(uint8_t generator, uint8_t aoi, uint8_t latch){
	// Use variables from accel_interrupt_generator, OR'd together to create
	// the [generator]value.
	uint8_t temp = generator;
	if (aoi) temp |= 0x80;
	if (latch) temp |= 0x40;
	SPIwriteByte(PIN_XG, INT_GEN_CFG_G, temp);
}

void configGyroThs(int16_t threshold, uint8_t axis, uint8_t duration, uint8_t wait){
	uint8_t buffer[2];
	buffer[0] = (threshold & 0x7F00) >> 8;
	buffer[1] = (threshold & 0x00FF);
	// Write threshold value to INT_GEN_THS_?H_G and  INT_GEN_THS_?L_G.
	// axis will be 0, 1, or 2 (x, y, z respectively)
	SPIwriteByte(PIN_XG, INT_GEN_THS_XH_G + (axis * 2), buffer[0]);
	SPIwriteByte(PIN_XG, INT_GEN_THS_XH_G + 1 + (axis * 2), buffer[1]);
	
	// Write duration and wait to INT_GEN_DUR_XL
	uint8_t temp;
	temp = (duration & 0x7F);
	if (wait) temp |= 0x80;
	SPIwriteByte(PIN_XG, INT_GEN_DUR_G, temp);
}

uint8_t availableGyro(void){
	uint8_t status = SPIreadByte(PIN_XG, STATUS_REG_1);
	return ((status & 0b00000010) >> 1);
}

	// Accelerometer

void readAccel(void){
	ax = (SPIreadByte(PIN_XG, OUT_X_H_XL) << 8 | SPIreadByte(PIN_XG, OUT_X_L_XL));
	ay = (SPIreadByte(PIN_XG, OUT_Y_H_XL) << 8 | SPIreadByte(PIN_XG, OUT_Y_L_XL));
	az = (SPIreadByte(PIN_XG, OUT_Z_H_XL) << 8 | SPIreadByte(PIN_XG, OUT_Z_L_XL));
}
	
	// Magnetometer

uint8_t getMagIntSrc(void){
	uint8_t intSrc = SPIreadByte(PIN_M, INT_SRC_M);
	
	// Check if the INT (interrupt active) bit is set
	if (intSrc & (1<<0)){
		return (intSrc & 0xFE);
	}
	
	return 0;
}

void configMagThs(uint16_t threshold){
	// Write high eight bits of [threshold] to INT_THS_H_M
	SPIwriteByte(PIN_M, INT_THS_H_M, ((threshold & 0x7F00) >> 8));
	// Write low eight bits of [threshold] to INT_THS_L_M
	SPIwriteByte(PIN_M, INT_THS_L_M, (threshold & 0x00FF));
}

void configMagInt(uint8_t generator, uint8_t activeLow, uint8_t latch){
	// Mask out non-generator bits (0-4)
	uint8_t config = (generator & 0xE0);
	// IEA bit is 0 for active-low, 1 for active-high.
	if (activeLow == 0) config |= (1<<2);
	// IEL bit is 0 for latched, 1 for not-latched
	if (!latch) config |= (1<<1);
	// As long as we have at least 1 generator, enable the interrupt
	if (generator != 0) config |= (1<<0);
	
	SPIwriteByte(PIN_M, INT_CFG_M, config);
}

void readMag(void){
	uint8_t temp[6]; // We'll read six bytes from the mag into temp
	SPIreadBytes(PIN_M, OUT_X_L_M, temp, 6);
	mx = (temp[1] << 8 | temp[0]);
	my = (temp[3] << 8 | temp[2]);
	mz = (temp[5] << 8 | temp[4]);
}

uint8_t availableMag(uint8_t axis){
	uint8_t status = SPIreadByte(PIN_M, STATUS_REG_M);
	return ((status & (1<<axis)) >> axis);
}
