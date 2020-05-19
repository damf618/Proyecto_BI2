/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/14
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "gpio.h"
#include "spi_hal.h"
#include "spi_prim.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

//Write Command in ASCII Write
uint8_t Write_Command[BUFF_SIZE]= {(uint8_t)119,(uint8_t)114,(uint8_t)105,(uint8_t)116,(uint8_t)101};
//Read Command in ASCII Read
uint8_t Read_Command[BUFF_SIZE]= {(uint8_t)114,(uint8_t)101,(uint8_t)97,(uint8_t)100,(uint8_t)95};
// All Ok! message in ASCII
uint8_t Check[BUFF_SIZE]= {(uint8_t)95,(uint8_t)79,(uint8_t)75,(uint8_t)33,(uint8_t)95};

/*=====[Definitions of private global variables]=============================*/

/*=====[Functions, program entry point after power on or reset]==========*/

void SPI_INIT(spi_Server_t * pServer1, spiMap_t spi, gpioMap_t pin){
	pServer1->spi=spi;
	pServer1->pin=pin;
	GPIOOutConfig(pServer1->pin);
	GPIOWrite(pServer1->pin,HIGH_G);
	spiInit2( pServer1->spi );
}

bool_t SPI_Server(spi_Server_t * pServer1, uint8_t * data)
{
	//charge data
	uint8_t status[BUFF_SIZE] ={0,0,0,0,0};
	//gpio off
	GPIOWrite(pServer1->pin,LOW_G);
	spiWrite2( pServer1->spi ,Write_Command,BUFF_SIZE);
	spiWrite2( pServer1->spi ,data,1);
	//Status read
	Write_Read_Command(pServer1->spi, Read_Command, BUFF_SIZE, status);
	//TODO Adaptar el protocoloUART aqui
	bool_t aux=1;
	for(char i=0;i<=BUFF_SIZE-1;i++)
	{
		if(status[i]!=Check[i])
			aux=0;
	}

	//gpio on
	GPIOWrite(pServer1->pin,HIGH_G);
	return aux;
}
