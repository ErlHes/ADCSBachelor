#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"

/*Defines*/
#define USART_BAUDRATE 9600
#define MYUBRR F_CPU/16/BAUD-1
#define PI 3.141592


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
	
	usart_init(MYUBRR);
	spiInit();
	_delay_ms(1000);
	WhoAmICheck();
	printf("Check complete, all systems are ready to go!\n");
	
	initMag();
	initGyro();
	calibrateMag(); 
	calibrateGyro();
	
	
	while(1){
			
		readGyro();
		readMag();
		
		angle_pitch += gx * 0.00007;
		angle_roll += gy * 0.00007;
		
		angle_pitch += angle_roll * sin(gz * 0.00007*PI/180);
		angle_roll -= angle_pitch * sin(gz * 0.00007*PI/180);
		
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
