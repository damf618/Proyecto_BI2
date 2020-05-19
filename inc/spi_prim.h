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
#define BUFF_SIZE 5
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
typedef struct{
	spiMap_t spi;
	gpioMap_t pin;
}spi_Server_t;

/*=====[Prototypes (declarations) of public functions]=======================*/
void SPI_INIT(spi_Server_t * pServer1, spiMap_t spi, gpioMap_t pin);
bool_t SPI_Server(spi_Server_t * pServer1, uint8_t * data);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_H_ */
