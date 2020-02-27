/*
 * gyroscope.c
 *
 * Created: 21.02.2020 14:42:41
 *  Author: Edvard
 */ 

void initGyro(void){

	//	reference value for digital high pass filter
	//REFERENCE_G = 0x00;
	spiWrite(CS_PIN, REFERENCE_G, 0x00);
	
	/*	bit 7-5:	output data rate selection, activates gyroscope when written
		bit 4-3:	full-scale selection
		bit 2:		always 0, don't write
		bit 1-0:	bandwidth selection */
	//CTRL_REG1_G = 0x00;
	spiWrite(CS_PIN, CTRL_REG1_G, 0b10000000);
	
	/*	bit 7-4:	always 0
		bit 3-2:	INT (interrupt) selection configuration
		bit 1-0:	OUT selection configuration	*/
	//CTRL_REG2_G = 0x00;
	spiWrite(CS_PIN, CTRL_REG2_G, 0x00);
	
	/*	bit 7:		low power mode enable
		bit 6:		high-pass filter enable
		bit 5-4:	always 0
		bit 3-0:	high-pass filter cutoff frequency selection*/
	//CTRL_REG3_G = 0x00;
	spiWrite(CS_PIN, CTRL_REG3_G, 0x00);
	
}


void interuptGyro(void){
	
}


void calibGyro(void){
	
}