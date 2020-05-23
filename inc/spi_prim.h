/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/12/09
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef SPI_PRIM_H_
#define SPI_PRIM_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include "sapi.h"


/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
#define BUFF_SIZE 32
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef struct{
	spiMap_t spi;
	gpioMap_t pin;
}spi_Server_t;

/*=====[Prototypes (declarations) of public functions]=======================*/
/** It sets initial conditions for the SPI Communication, it establish the default values
 *  of SPI Mode and SS Pin.

	@param pServer1 element of type *spi_Server_t* with all SPI data types needed.
	@param spi SPI Port to be used for the Monitor Program.
	@param pin selected for the SS function.
	@note The entire protocol is based on the SPISlave.h protocol for the NodeMCU.

**/
void SPI_INIT(spi_Server_t * pServer1, spiMap_t spi, gpioMap_t pin);

/** 2 Modes of working, Depending of the previous action taken.
 * Mode A we sent the Writing Command. We ask for the NodeMCU to upload the
 * current state to the Firebase Database.
 * Mode B we sent the Reading Command. We read if the NodeMCU could upload
 * the info correctly or not.

	@param pServer1 element of type *spi_Server_t* with all SPI data types needed.
	@param data data to be written in the Mode A of operation, it will be written after the
	":"character.
	@param readv vector to receive the answer of the SPI slave, it will save result
	of the server upload operation.
	@note The entire protocol is based on the SPISlave.h protocol for the NodeMCU.
	This function must be called after SPI_INIT.
	@see SPI_INIT.

**/
bool_t SPI_Server(spi_Server_t * pServer1, uint8_t data,uint8_t * readv);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_H_ */
