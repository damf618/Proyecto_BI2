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
bool_t reading=FALSE;
/* All OK! message in ASCII
                           _   O  K  !  _ null				*/
uint8_t Check[BUFF_SIZE]= {95,79,75,33,95,0};

/*=====[Definitions of private global variables]=============================*/

/*=====[Functions, program entry point after power on or reset]==========*/

void SPI_INIT(spi_Server_t * pServer1, spiMap_t spi, gpioMap_t pin){

	pServer1->spi=spi;							//Set the SPI PORT
	pServer1->pin=pin;							//Set the SS Pin
	GPIOOutConfig(pServer1->pin);				//Configure the SS Pin as Slave
	GPIOWrite(pServer1->pin,HIGH_G);			//Release the Slave
	spiInit2( pServer1->spi );					// Configuration of the SPI Parameters
}

bool_t SPI_Server(spi_Server_t * pServer1, uint8_t data, uint8_t * readv )
{
	bool_t aux=TRUE;
	//Enable the Slave SSPin LOW
	GPIOWrite(pServer1->pin,LOW_G);

	//TODO SPlit this in 2 tasks 1 for reading and one for writing, we will be
	//needing a mutex for the protection of the SPI resource.

	//Flag for SPI Reading
	if(reading){
		uint8_t Read_Command[BUFF_SIZE]= {3,0};		//For Reading the ESP8266 needs to receive the bytes 3 and after that a 0
		spiWrite2( pServer1->spi ,Read_Command,2);	//After that we get the MISO signal.
		spiRead2( pServer1->spi ,readv,32);			//WARNING The length of the SPI Message is 32 bytes
		reading=FALSE;								// clear the flag.

	}
	else{
		//Write Command in ASCII Write	 //     w   r   i   t   e  _  c   u   r   r   e   n   t  ""  s   t   a  t  e   :
		uint8_t Write_Command[BUFF_SIZE]= {2,0,119,114,105,116,101,95,99,117,114,114,101,110,116,32,115,116,97,116,101,58,data+48};
		spiWrite2( pServer1->spi ,Write_Command,32);	//read the command and the current state as a byte with 6 states
		for(char i=0;i<=5-1;i++)
		{
			if(readv[i]!=Check[i])						//Did we get an _OK!_ answer?
				aux=FALSE;
		}
		reading=TRUE;									//set flag
	}
	GPIOWrite(pServer1->pin,HIGH_G);				//Release the SPI Slave

	return aux;
}
