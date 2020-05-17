#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
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
	uint8_t counter = 0;
	stdout = &mystdout; // related to printf
	
	// gyro sample rate [Hz]: choose value between 1-6
	// 1 = 14.9    4 = 238
	// 2 = 59.5    5 = 476
	// 3 = 119     6 = 952
	gyroSampleRate = 2;	// remember to set in madgwick.c as well!
	
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
	initAccel();
	calibrateOffsetMag(-1158, 3935, 224); // Sets offset, calculated in Matlab function.
//	calibrateOffsetMag(0, 0, 0); // Reset offset
	calibrateGyro(); // Calculates the average offset value the gyro measures. IMU must be held still during this.
	calibrateAccel();
	
	uint16_t timerticks = runTime(gyroSampleRate);	// Sets the runtime for the repeating loop.
	uint16_t temp = 0;
	
	timerInit(); // Start the clock
	TCNT1 = 0x0000; // Make it start at 0.... this is pointless now.
	
	while(1){
		counter += 1;
		readMag();
		// convert magnetometer data to Gauss:
		mag_x = mx * SENSITIVITY_MAGNETOMETER_4;
		mag_y = my * SENSITIVITY_MAGNETOMETER_4;
		mag_z = mz * SENSITIVITY_MAGNETOMETER_4;
		// compensate for soft iron distortion using values from Matlab: 
		softIronMag(0.98589, 1.00022, 1.01526, 0.03315, 0.00162, 0.00605, -0.0062, 0.0025, 0.0028);
//		mag_x = 0;	mag_y = 0;	mag_z = 0;		// for using madgwick without magnetometer
				
		readGyro();
		gx -= gBiasRawX;
		gy -= gBiasRawY;
		gz -= gBiasRawZ;	
		// convert gyroscope data to rad/s:
		gyro_x = gx * (SENSITIVITY_GYROSCOPE_500 * (PI / 180));
		gyro_y = gy * (SENSITIVITY_GYROSCOPE_500 * (PI / 180));
		gyro_z = gz * (SENSITIVITY_GYROSCOPE_500 * (PI / 180));
		
		readAccel();
		ax -= aBiasRawX;
		ay -= aBiasRawY;
		az -= aBiasRawZ;	
		// convert accelerometer data to g:
		acc_x = ax * SENSITIVITY_ACCELEROMETER_8;	
		acc_y = ay * SENSITIVITY_ACCELEROMETER_8;	
		acc_z = az * SENSITIVITY_ACCELEROMETER_8;	

		
		MadgwickAHRSupdate(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z);	
		QuaternionsToEuler(q0, q1, q2, q3);
		
		//TEST STUFF
		//PRINTING AV FORRIGE FØRSTE VINKEL MÅ SKJE FØR NESTE RUNDE MED QUATERNIONS TO EULER
		
		// convert angles from radians to degrees:
		angle_pitch *= (180/PI);
		angle_roll *= (180/PI);
		angle_yaw *= (180/PI);
				
		//PRINT HER
		printf("Roll0.1:	%f\t", angle_roll);
		
		
		MadgwickAHRSupdate2(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z);
		QuaternionsToEuler2(q00, q01, q02, q03);
				
		// convert angles from radians to degrees:
		angle_pitch *= (180/PI);
		angle_roll *= (180/PI);
		angle_yaw *= (180/PI);
		
		//PRINT "SAMME" VINKEL PÅ NYTT HER.	
		printf("roll_1:	%f\n", angle_roll);
				
		if(counter == 1){
//			printf("q0:	%f\t", q0);
//			printf("q1:	%f\t", q1);
//			printf("q2:	%f\t", q2);
//			printf("q3:	%f\n", q3);
//			printf("Pitch:	%f\t", angle_pitch);	 
//			printf("Roll:	%f\t", angle_roll);
//			printf("Yaw:	%f\n", angle_yaw);
//			printf("mx: %f\t", mag_x);
//			printf("my: %f\t", mag_y);
//			printf("mz: %f\n", mag_z);
//			printf("gx: %f\t", gyro_x);
//			printf("gy: %f\t", gyro_y);
//			printf("gz: %f\n", gyro_z);
//			printf("ax: %f\t", acc_x);
//			printf("ay: %f\t", acc_y);
//			printf("az: %f\n", acc_z);
			counter = 0;
		} 
		
		// makes sure the program runs at correct speed
		if((TCNT1 > timerticks) | (TIFR1 & (1<<TOV1))) { 
			temp = TCNT1;
			printf("Game over! You were too slow! \n");
			printf("Clock cycles lapsed: %u\n", temp);
			printf("Clock cycles limit: %u\n", timerticks);
			while(1); // stop the program.
		}
		while(TCNT1 < timerticks);	
		// Wait for the next gyro sample to be ready
		TCNT1 = 0x0000; // Reset the timer
		



		
		
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
