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


/*=====[Prototypes (declarations) of public functions]=======================*/
/** It sets the initial conditions for the use of the SPI protocol.

	@param spi element of type *spiMap_t* with the spi related data types needed
	@note It is based on the sapi library.

**/
void spiInit2( spiMap_t spi );

/** It reads the SPI Port and saves the data in the designated buffer.

	@param spi element of type *spiMap_t* with the spi related data types needed
	@param buffer vector/array to save the message from the SPI Port.
	@param bufferSize length of the expected SPI message.
	@note It is based on the sapi library.

**/
bool_t spiRead2( spiMap_t spi, uint8_t* buffer, uint32_t bufferSize );

/** It writes on the SPI Port the message allocated in the designated buffer.

	@param spi element of type *spiMap_t* with the spi related data types needed
	@param buffer vector/array that contains the message to be written on the SPI Port.
	@param bufferSize length of the SPI message.  t
	@note It is based on the sapi library.

**/
bool_t spiWrite2( spiMap_t spi, uint8_t* buffer, uint32_t bufferSize);

/** It writes on the SPI Port the message allocated in the designated buffer, and also reads
 * in case there an answer from the slave at the same time.

	@param spi element of type *spiMap_t* with the spi related data types needed
	@param wbuffer vector/array that contains the message to be written on the SPI Port.
	@param rbuffer vector/array to save the message from the SPI Port.
	@param bufferSize length of the SPI message.
	@note It is based on the sapi library.

**/
void Write_Read_Command(spiMap_t spi, uint8_t* wbuffer, uint32_t bufferSize, uint8_t* rbuffer);

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* SPI_HAL_H_ */
