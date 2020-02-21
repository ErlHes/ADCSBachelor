/*
 * IncFile1.h
 *
 * Created: 20.02.2020 11:37:05
 *  Author: Edvard
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

#define ACT_THS 0x04	// R/W

void SPI_init_master(void);

/* SPIread - reads one byte of data from the desired registry via the hardware SPI.
	INPUTS:
		-csPin = chip select, chose between Accelerometer/Gyroscope or Magnetometer
		-subAdress = the desired registry to read from
*/
uint8_t SPIRead(uint8_t csPin, uint8_t subAddress);	


/* SPIWrite - writes one byte of data to desired registry via SPI
	INPUTS:
		-csPin = chip select, chose between Accelerometer/Gyroscope or Magnetometer
		-subAdress = the desired registry to write to
		-data = byte to send
*/
void SPIWrite(uint8_t csPin, uint8_t subAddress, uint8_t data);

#endif /* INCFILE1_H_ */