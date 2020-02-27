
/* Functions to initialize, send, receive over USART

   initUSART requires BAUD to be defined in order to calculate
     the bit-rate multiplier.
 */

#ifndef BAUD                          /* if not defined in Makefile... */
#define BAUD  9600                     /* set a safe default baud rate */
#endif

#define PIN_XG PB1
#define PIN_M PB2

                                  /* These are defined for convenience */
#define   USART_HAS_DATA   bit_is_set(UCSR0A, RXC0)
#define   USART_READY      bit_is_set(UCSR0A, UDRE0)

/* Takes the defined BAUD and F_CPU,
   calculates the bit-clock multiplier,
   and configures the hardware USART                   */
void initUSART(void);

/* Blocking transmit and receive functions.
   When you call receiveByte() your program will hang until
   data comes through.  We'll improve on this later. */
void transmitByte(uint8_t data);
uint8_t receiveByte(void);

void printString(const char myString[]);
             /* Utility function to transmit an entire string from RAM */
void readString(char myString[], uint8_t maxLength);
/* Define a string variable, pass it to this function
   The string will contain whatever you typed over serial */

void printByte(uint8_t byte);
                  /* Prints a byte out as its 3-digit ascii equivalent */
void printWord(uint16_t word);
        /* Prints a word (16-bits) out as its 5-digit ascii equivalent */

void printBinaryByte(uint8_t byte);
                                     /* Prints a byte out in 1s and 0s */
char nibbleToHex(uint8_t nibble);
char nibbleToHexCharacter(uint8_t nibble);
void printHexByte(uint8_t byte);
                                   /* Prints a byte out in hexadecimal */
uint8_t getNumber(void);


/*  constrainScales - Constrains the scales of all sensors to make sure they're not out of spec, defaults the value to lowest valid scale.
*/
void constrainScales(void);


/* spiInit - Initializes the SPI.
*/
void spiInit(void);


/* initGyro - Initializes the Gyroscope control registers.
*/
void initGyro(void);


/* interuptGyro - Initializes the gyroscope interrupt registers.
*/
void interuptGyro(void);


/* calibGyro - Calibrates the gyroscope on the IMU module, see gyroscope.c
*/
void calibGyro(void);


/* initMag - Initializes the magnetometer control registers, values can be changed in magnetometer.c
*/
void initMag(void);


/* interruptMap - Sets up the interrupt flags for magnetometer, set values in magnetometer.c
*/
void interuptMag(void);


/* offsetMag - initializes the magnetometer offsets, values can be changed in magnetometer.c

*/
void offsetMag(void);


/* SPIreadByte - reads one byte of data from the desired registry via the hardware SPI.
	INPUTS:
		-csPin = chip select, chose between Accelerometer/Gyroscope or Magnetometer
		-subAdress = the desired registry to read from
*/
uint8_t SPIreadByte(uint8_t csPin, uint8_t subAddress);


/* SPIreadBytes - reads a desired amount of bytes, incrementing the target registry by one for every read.
	INPUTS:
		-csPin = Chip Select, chose between XL/G or M
		-subADress = The desired start address
		-dest = Destination array for storing data
		-count = desired amount of bytes to read
*/
uint8_t SPIreadBytes(uint8_t csPin, uint8_t subAddress,
uint8_t * dest, uint8_t count);


/* SPIwriteByte - writes one byte of data to desired registry via SPI
	INPUTS:
		-csPin = chip select, chose between Accelerometer/Gyroscope or Magnetometer
		-subAdress = the desired registry to write to
		-data = byte to send
*/
void SPIwriteByte(uint8_t csPin, uint8_t subAddress, uint8_t data);


/* WhoAmICheck - Checks the WHO_AM_I registers of both magnetometer and Accelerometer/Gyro
				 to see if they match the expected response, holds the program until the check passes.
*/
void WhoAmICheck(void);

/* spiTransfer - Transmits one byte of data over SPI, reads one byte back.
	-data = Byte to send.
*/
uint8_t spiTransfer(uint8_t data);