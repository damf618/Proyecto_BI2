/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/12/09
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef SPI_HAL_H_
#define SPI_HAL_H_
/*=====[Inclusions of public function dependencies]==========================*/

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/
void spiInit2( spiMap_t spi );
bool_t spiRead2( spiMap_t spi, uint8_t* buffer, uint32_t bufferSize );
bool_t spiWrite2( spiMap_t spi, uint8_t* buffer, uint32_t bufferSize);
void Write_Read_Command(spiMap_t spi, uint8_t* wbuffer, uint32_t bufferSize, uint8_t* rbuffer);

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* SPI_HAL_H_ */
