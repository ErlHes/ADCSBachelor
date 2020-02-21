/*
 * testspii.c
 *
 * Created: 20.02.2020 10:49:14
 * Author : Edvard
 */ 

#include <avr/io.h>
#include "main.h"
#include "registers.h"

int main(void)
{
	SPI_init_master();
	
	uint8_t G_X_L = SPIRead(PINB1, OUT_X_L_G);
	uint8_t G_X_H = SPIRead(PINB1, OUT_X_H_G);
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

