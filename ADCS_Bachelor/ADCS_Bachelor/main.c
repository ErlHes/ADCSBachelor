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
	stdout = &mystdout; // related to printf
	
	// mag scale can be 4, 8, 12, or 16
	magScale = 4;
	
	// Gyroscope scaling, choose: 0 = 245dps, 1 = 500dps, 3 = 2000dps
	gyroScale = 1;
	
	autocalc = 1; // Set autocalc enable -- This should always be on
	
	usart_init(MYUBRR);
	spiInit();
	timerInit();
//	_delay_ms(1000);	// Magic 1 second delay that can be removed later to speed up the program :-)
	WhoAmICheck();		// Checks if the wires and SPI are correctly configured, if this check can't complete the program will not continue, this is to protect the IMU.
	printf("Check complete, all systems are ready to go!\n");
	
	initMag();  // Sets the magnetometer control registers, settings can be changed in sensorInits.c
	initGyro(); // Sets the gyroscope control registers, settings can be changed in sensorInits.c
	initAccel();
	calibrateMag(); // Calculates the median offset value the magnetometer measures.
	calibrateGyro(); // Calculates the average offset value the gyro measures. IMU must be held still during this.
	calibrateAccel();
	uint16_t temp = 0;
	
	TCNT1 = 0x00; // Set the timer.

	while(1){
		readMag();
		// convert magnetometer data to Gauss:
		mag_x = mx * SENSITIVITY_MAGNETOMETER_4;
		mag_y = my * SENSITIVITY_MAGNETOMETER_4;
		mag_z = mz * SENSITIVITY_MAGNETOMETER_4;		
				
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
				
		// 0.00014706 = (1/ 14,9) * 0.0175
//		angle_pitch += gx * 0.0011744966; 
//		angle_roll += gy * 0.0011744966;
		
//		angle_pitch -= angle_roll * sin(gz * 0.0011744966 * (PI / 180)); // Transfer roll to pitch in case of yaw
//		angle_roll += angle_pitch * sin(gz * 0.0011744966 * (PI / 180)); // Transfer pitch to roll in case of yaw
		
		// Accelerometer angle calculations
		// a_total_vector = sqrt((ax*ax)+(ay*ay)+(az*az));
		// printf("a_total_vector = %u\n", a_total_vector); 
		
		MadgwickAHRSupdate(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z);	
		QuaternionsToEuler(q0, q1, q2, q3);
		
		// convert angles from radians to degrees:
		angle_pitch *= (180/PI);
		angle_roll *= (180/PI);
		angle_yaw *= (180/PI);
		
		
//		angle_pitch_acc = asin((float)ay/4096) * 57.296; //4096 is an approximation
//		angle_roll_acc = asin((float)ax/4096) * -57.296; // --||--
		
//		angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;     //Correct the drift of the gyro pitch angle with the accelerometer pitch angle
//		angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;        //Correct the drift of the gyro roll angle with the accelerometer roll angle
		
			
//		printf("q0:	%f\n", q0);
//		printf("Pitch:	%f\n", angle_pitch);	
//		printf("Roll:	%f\n", angle_roll);
//		printf("yaw:	%f\n", angle_yaw);
//		printf("clockticks:	%u\n", temp);
		while(1);	
		
		// should be 16779 (67,1 milliseconds)
		if(TCNT1 > 16779){ 
			temp = TCNT1;
			printf("Game over! you were too slow! ");
			printf("Clock cycles lapsed: %u\n", temp);
			while(1);
		}
		while(TCNT1 < 16779);
		
		TCNT1 = 0x0000;
		
		/*
		temp = TCNT1;
		printf("\n");
		printf("Waited %u clock cycles\n", temp);
		printf("\n");
		*/
		
		
		/*
		temp = TCNT1;
		printf("\n");
		printf("Clock cycles after reset: %u\n", temp);
		printf("\n");		
		*/
		
		
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
