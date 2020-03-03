#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "header.h"
#include "registers.h"

/*Defines*/
#define USART_BAUDRATE 9600
#define MYUBRR F_CPU/16/BAUD-1

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);


int main(void)
{
	stdout = &mystdout; // related to printf
	
	usart_init(MYUBRR);
	spiInit();
	_delay_ms(1000);
	WhoAmICheck();
	printf("Check complete, all systems are ready to go!\n");

	float gx;
	float gy;
	float gz;
	float mx;
	float my;
	float mz;
	initMag();
	initGyro();
	calibrateMag(1); 
	int16_t gxBias = calibrateGyro(OUT_X_L_G);
	int16_t gyBias = calibrateGyro(OUT_Y_L_G);
	int16_t gzBias = calibrateGyro(OUT_Z_L_G);
	while(1){

		int16_t temp = 0;
		temp = readGyro_calc(OUT_X_L_G, gxBias);
		gx = calcGyro(temp);
		temp = readGyro_calc(OUT_Y_L_G, gyBias);
		gy = calcGyro(temp);
		temp = readGyro_calc(OUT_Z_L_G, gzBias);
		gz = calcGyro(temp);
				
		temp = readMag(OUT_X_L_M);
		mx = calcMag(temp);
		temp = readMag(OUT_Y_L_M);
		my = calcMag(temp);
		temp = readMag(OUT_Z_L_M);
		mz = calcMag(temp);
		
		
		printf("Reading Gyroscpe: \n");
		printf("X: %f ", gx);
		printf("Y: %f ", gy);
		printf("Z: %f \n", gz);
		printf("\n");

		
		printf("Reading Magnetometer: \n");
		printf("X: %f ", mx);
		printf("Y: %f ", my);
		printf("Z: %f \n", mz);
		printf("\n");
		
		_delay_ms(100);
	}
}
