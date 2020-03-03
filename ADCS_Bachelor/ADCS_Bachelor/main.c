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

	initMag();
	initGyro(); 
	while(1){

		int16_t temp = 0;
		temp = readGyro(OUT_X_L_G);
		gx = calcGyro(temp);

		temp = readGyro(OUT_Y_L_G);
		gy = calcGyro(temp);

		temp = readGyro(OUT_Z_L_G);
		gz = calcGyro(temp); 
	
		printf("Reading Gyroscpe: \n");
		printf("X: %f ", gx);
		printf("Y: %f ", gy);
		printf("Z: %f \n", gz);
		printf("\n");
		
		_delay_ms(100);
	}
}
