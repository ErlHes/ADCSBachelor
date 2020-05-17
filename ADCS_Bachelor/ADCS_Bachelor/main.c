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
	
	// Interfacing inits:
	usart_init(MYUBRR);
	spiInit();
	WhoAmICheck();		// Checks if the wires and SPI are correctly configured, if this check can't complete the program will not continue, this is to protect the IMU.
	
	printf("Check complete, all systems are ready to go!\n");
	
	// Sensor inits:
	initMag();  // Sets the magnetometer control registers, settings can be changed in sensorInits.c.
	initGyro(); // Sets the gyroscope control registers, settings can be changed in sensorInits.c
	initAccel();
	
	// Sensor Calibration:
	calibrateOffsetMag(-947, 3720, 175); // Sets offset, calculated in Matlab function, see readme
	calibrateGyro(); // Calculates the average offset value the gyro measures. IMU must be held still during this.
	calibrateAccel();
	
	// Timer inits:
	uint16_t timerticksMax = runTime(gyroSampleRate);	// Sets the runtime for the repeating loop.
	uint16_t TimerValue = 0;
	timerInit(); // Start the timer.
	
	while(1){
		counter += 1;	// Counter to keep track of iteration count, this is for when you don't want to print out data every iteration.
		readMag();
		// convert magnetometer data to Gauss:
		mag_x = mx * SENSITIVITY_MAGNETOMETER_4;	// Be sure to use the correct sensitivity factor.
		mag_y = my * SENSITIVITY_MAGNETOMETER_4;
		mag_z = mz * SENSITIVITY_MAGNETOMETER_4;
		// compensate for soft iron distortion using values from Matlab: 
		softIronMag(0.99847, 0.98362, 1.01898, 0.02723, 0.00005, 0.00311, -0.00084, -0.00821, -0.00468);
//		mag_x = 0;	mag_y = 0;	mag_z = 0;		// For using madgwick without magnetometer.
				
		readGyro();
		// Subtract the Offset value.
		gx -= gBiasRawX;
		gy -= gBiasRawY;
		gz -= gBiasRawZ;	
		// convert gyroscope data to rad/s:
		gyro_x = gx * (SENSITIVITY_GYROSCOPE_500 * (PI / 180));		// Be sure to use the correct sensitivity factor.
		gyro_y = gy * (SENSITIVITY_GYROSCOPE_500 * (PI / 180));
		gyro_z = gz * (SENSITIVITY_GYROSCOPE_500 * (PI / 180));
		
		readAccel();
		// Subtract the Offset value.
		ax -= aBiasRawX;
		ay -= aBiasRawY;
		az -= aBiasRawZ;	
		// convert accelerometer data to g:
		acc_x = ax * SENSITIVITY_ACCELEROMETER_8;	// Be sure to use the correct sensitivity factor.
		acc_y = ay * SENSITIVITY_ACCELEROMETER_8;	
		acc_z = az * SENSITIVITY_ACCELEROMETER_8;	

		
		MadgwickAHRSupdate(gyro_x, gyro_y, gyro_z, acc_x, acc_y, acc_z, mag_x, mag_y, mag_z);	// Run data through the Madgwick filter, this outputs the attitude as Quaternions
		QuaternionsToEuler(q0, q1, q2, q3);														// Convert back to Euler angles.
		
		// convert angles from radians to degrees:
		angle_pitch *= (180/PI);
		angle_roll *= (180/PI);
		angle_yaw *= (180/PI);
		
				
		if(counter == 1){				// Edit comparison value to set how often you want to print out data.
			
			// Print Quaternions.
//			printf("q0:	%f\t", q0);
//			printf("q1:	%f\t", q1);
//			printf("q2:	%f\t", q2);
//			printf("q3:	%f\n", q3);
			
			// Print Euler angles.
			printf("Pitch:	%f\t", angle_pitch);	 
			printf("Roll:	%f\t", angle_roll);
			printf("Yaw:	%f\n", angle_yaw);
			
			// Print individual sensor outputs.
//			printf("mx: %f\t", mag_x);
//			printf("my: %f\t", mag_y);
//			printf("mz: %f\n", mag_z);
//			printf("gx: %f\t", gyro_x);
//			printf("gy: %f\t", gyro_y);
//			printf("gz: %f\n", gyro_z);
//			printf("ax: %f\t", acc_x);
//			printf("ay: %f\t", acc_y);
//			printf("az: %f\n", acc_z);

			counter = 0;				// Reset the counter.
		} 
		
		// This code checks if the program ran fast enough to keep up with the Gyroscope ODR.
		if((TCNT1 > timerticksMax) | (TIFR1 & (1<<TOV1))){ 
			TimerValue = TCNT1;
			printf("Game over! You were too slow! \n");
			if(TIFR1 & (1<<TOV1)) printf("Timer overflow happened\r\n");
			printf("Clock cycles lapsed: %u\n", TimerValue);
			printf("Clock cycles limit: %u\n", timerticksMax);
			while(1);
		}
		while(TCNT1 < timerticksMax);		// Wait for new Gyroscope sample to be ready
		TCNT1 = 0x0000;					// Reset the clock and go again.
	}
}
