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

/*=====[Definitions of public global variables]==============================*/
bool_t reading=FALSE;
/* All OK! message in ASCII
------------------------------------------------------------------------------
                           _   O  K  !  _ null
------------------------------------------------------------------------------*/
uint8_t Check[BUFF_SIZE]= {95,79,75,33,95,0};

/*=====[Functions, program entry point after power on or reset]==========*/

// SPI Initialization for the Principal Device
void SPI_INIT(spi_Server_t * pServer1, spiMap_t spi, gpioMap_t pin, gpioMap_t resetpin){

	pServer1->spi=spi;							//Set the SPI PORT
	pServer1->pin=pin;							//Set the SS Pin
	pServer1->RSTpin=resetpin;					//Set RST Pin
	GPIOOutConfig(pServer1->pin);				//Configure the SS Pin as Slave as Output
	GPIOWrite(pServer1->pin,HIGH_G);			//Release the Slave
	GPIOOutConfig(pServer1->RSTpin);			//Configure the Reset Slave SPI Pin as Output
	GPIOWrite(pServer1->RSTpin,HIGH_G);			//set the Reset Slave SPI Pin as HIGH
	spiInit2( pServer1->spi );					// Configuration of the SPI Parameters
}

// SPI Write to the SPI SLave to command the update of the state into the web server
void SPI_ServerW(spi_Server_t * pServer1, uint8_t data )
{
	//Enable the Slave SSPin LOW
	GPIOWrite(pServer1->pin,LOW_G);

	//Write Command in ASCII Write	 //     w   r   i   t   e  _  c   u   r   r   e   n   t  ""  s   t   a  t  e   :
	uint8_t Write_Command[BUFF_SIZE]= {2,0,119,114,105,116,101,95,99,117,114,114,101,110,116,32,115,116,97,116,101,58,data+48};
	spiWrite2( pServer1->spi ,Write_Command,32);	//write the command and the current state as a byte with 6 states

	GPIOWrite(pServer1->pin,HIGH_G);				//Release the SPI Slave

}

// SPI read from the SPI SLave to check if the update of the state was made correctly
bool_t SPI_ServerR(spi_Server_t * pServer1, uint8_t * readv )
{
	bool_t aux=TRUE;
	GPIOWrite(pServer1->pin,LOW_G);					//Enable the Slave SSPin LOW
	uint8_t Read_Command[BUFF_SIZE]= {3,0};			//For Reading the ESP8266 needs to receive the bytes 3 and after that a 0
	spiWrite2( pServer1->spi ,Read_Command,2);		//After that we get the MISO signal.
	spiRead2( pServer1->spi ,readv,32);				//WARNING The length of the SPI Message is 32 bytes

	for(char i=0;i<=5-1;i++)
	{
		if(readv[i]!=Check[i])						//Did we get an _OK!_ answer?
			aux=FALSE;
	}
	GPIOWrite(pServer1->pin,HIGH_G);				//Release the SPI Slave
	return aux;
}

// The SPI Slave (NodeMCU) needs a particular sequence for booting
void reset_SPI(spi_Server_t * pServer1, uint8_t stage )
{
	if(stage==PRE_BOOTING){
		GPIOWrite(pServer1->pin,LOW_G);						//Slave SSPin LOW, FOR ESP8266 BOOTING
		GPIOWrite(pServer1->RSTpin,LOW_G);					//RESET Pin LOW
	}
	else if(stage==BOOTING){
		GPIOWrite(pServer1->pin,LOW_G);						//Slave SSPin LOW, FOR ESP8266 BOOTING
		GPIOWrite(pServer1->RSTpin,HIGH_G);					//RESET Pin HIGH, Booting Stage
	}
	else if (stage==POST_BOOTING){
		GPIOWrite(pServer1->pin,HIGH_G);					//Slave SSPin HIGH
		GPIOWrite(pServer1->RSTpin,HIGH_G);					//NORMAL
	}
}

