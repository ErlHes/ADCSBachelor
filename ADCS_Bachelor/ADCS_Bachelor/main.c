#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"

//************* DECLINATION MIGHT NOT MATTER AFTER ALL.********************************//

// Gotta consider declination
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -3.58 // Declination (degrees) Trondheim = 3° 52' E  ± 0° 29'  changing by  0° 14' E per year

//************************************************************************************//


static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);


// Global variables are made in the header //



int main(void)
{
	stdout = &mystdout; // related to printf
	
	// mag scale can be 4, 8, 12, or 16
	magScale = 4;
	
	// Gyroscope scaling, choose: 0 = 245dps, 1 = 500dps, 3 = 2000dps
	gyroScale = 1;
	
	autocalc = 1; // Set autocalc enable -- This should always be on
	
	usart_init(MYUBRR);
	spiInit();
	_delay_ms(1000);	// Magic 1 second delay that can be removed later to speed up the program :-)
	WhoAmICheck();		// Checks if the wires and SPI are correctly configured, if this check can't complete the program will not continue, this is to protect the IMU.
	printf("Check complete, all systems are ready to go!\n");
	
	initMag();  // Sets the magnetometer control registers, settings can be changed in sensorInits.c
	initGyro(); // Sets the gyroscope control registers, settings can be changed in sensorInits.c
	calibrateMag(); // Calculates the average offset value the gyro measures. IMU must be held still during this.
	calibrateGyro(); // Calculates the median offset value the magnetometer measures.
	

	while(1){
		
		// TODO
		// * Finish refactoring code
		// * Clean up spaghetti code from the refactoring
		// * Comment Code - Go through all the code with the group, get everyone on the same page.
		// * Implement hardware counter to configure pulse, the program should loop every 4000 microsecond, or with 250hz. if possible.
		// * If the program can not pulse properly, a timer would still be needed to calculate the pitch and roll.
		// * The accelerometer is essential to avoid gyroscope drift, better bite the bullet and set it up early.
		// * Learn how to use the magnetometer to find calculate heading
		// * Implement magnetometer heading calculations
		// * Convert everything to radians
		// * Display data in a meaningful manner
		
		// TODO (later)
		// * Conduct static measurement tests to find noise characteristics
		// * Conduct dynamic measurement tests to find more noise
		// * Signal processing
		// * ?????
		// * A+ 
		
		
		
		//*********************************************************************//
		//																	   //
		//						HEADING / PITCH / ROLL / YAW				   //
		//																	   //
		//*********************************************************************//
		
	
				
	}
}
