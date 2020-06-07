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

#define PRE_BOOTING 0
#define BOOTING 1
#define POST_BOOTING 2

/*=====[Definitions of public data types]====================================*/
typedef struct{
	spiMap_t spi;
	gpioMap_t pin;
	gpioMap_t RSTpin;
}spi_Server_t;

/*=====[Prototypes (declarations) of public functions]=======================*/
/** It sets initial conditions for the SPI Communication, it establish the default values
 *  of SPI Mode and SS Pin.

	@param pServer1 element of type *spi_Server_t* with all SPI data types needed.
	@param spi SPI Port to be used for the Monitor Program.
	@param pin selected for the SS function.
	@param pin selected for the RESET function.
	@note The entire protocol is based on the SPISlave.h protocol for the NodeMCU.

**/
void SPI_INIT(spi_Server_t * pServer1, spiMap_t spi, gpioMap_t pin,gpioMap_t resetpin);

/** Sends the Writing Command (0x02, 0x0), asking the NodeMCU to upload the
 * current state to the Firebase Database.

	@param pServer1 element of type *spi_Server_t* with all SPI data types needed.
	@param data data to be written in the Mode A of operation, it will be written after the
	":"character.
	@note The entire protocol is based on the SPISlave.h protocol for the NodeMCU.
	This function must be called after SPI_INIT.
	@see SPI_INIT.

**/
void SPI_ServerW(spi_Server_t * pServer1, uint8_t data);

/** Sends the Reading Command (0x03, 0x0). We read if the NodeMCU could upload
 * the info correctly or not.


	@param pServer1 element of type *spi_Server_t* with all SPI data types needed.
	@param readv vector to receive the answer of the SPI slave, it will save result
	of the server upload operation.
	@note The entire protocol is based on the SPISlave.h protocol for the NodeMCU.
	This function must be called after SPI_INIT, it should be called after a write was made to
	work properly.
	@see SPI_INIT.

**/
bool_t SPI_ServerR(spi_Server_t * pServer1,uint8_t * readv);

/** The NodeMCU requires an specific gpio states to run it's booting sequence, it requires
 * the SS Pin of the SPI port to be turned off, but to allowthesystem work properly itmust be
 *  set back on once the device is stable. The booting sequence was splitted in 3 stages.
 *  Stage1 - Pre-Booting: Turn Slave's Power Control pin off and SS Pin off.
 *  Stage2 - Booting: Turn Slave's Power Control pin on and SS Pin off.
 *  Stage3 - Post-Booting:  Turn Slave's Power Control pin on and SS Pin on.

	@param pServer1 element of type *spi_Server_t* with all SPI data types needed.
	@param stage desired stage of the booting sequence.
	@note The entire protocol is based on the SPISlave.h protocol for the NodeMCU. It does not
	change of stages automatically.
	@see Reset_Slave.

**/
void reset_SPI(spi_Server_t * pServer1,uint8_t stage );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_H_ */
