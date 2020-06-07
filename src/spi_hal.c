/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/16
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/
#include "sapi.h"
/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Functions, program entry point after power on or reset]==========*/

void spiInit2( spiMap_t spi )
{
         /* Set up clock and power for SSP1 module */
         // Configure SSP SSP1 pins
         Chip_SCU_PinMuxSet( 0xF, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC0)); // SSP1_SCK
         Chip_SCU_PinMuxSet( 0x1, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5)); // SSP1_MISO
         Chip_SCU_PinMuxSet( 0x1, 4, (SCU_MODE_PULLUP | SCU_MODE_FUNC5)); // SSP1_MOSI

         // Initialize SSP Peripheral
         Chip_SSP_Init( LPC_SSP1 );
         Chip_SSP_Enable( LPC_SSP1 );
}


bool_t spiRead2( spiMap_t spi, uint8_t* buffer, uint32_t bufferSize )
{

   bool_t retVal = TRUE;

   Chip_SSP_DATA_SETUP_T xferConfig;

   xferConfig.tx_data = NULL;
   xferConfig.tx_cnt  = 0;
   xferConfig.rx_data = buffer;
   xferConfig.rx_cnt  = 0;
   xferConfig.length  = bufferSize;

   if( spi == SPI0 ) {
      Chip_SSP_RWFrames_Blocking( LPC_SSP1, &xferConfig );
   } else {
      retVal = FALSE;
   }

   return retVal;
}


bool_t spiWrite2( spiMap_t spi, uint8_t* buffer, uint32_t bufferSize)
{

   bool_t retVal = TRUE;
   Chip_SSP_DATA_SETUP_T xferConfig;

   xferConfig.tx_data = buffer;
   xferConfig.tx_cnt  = 0;
   xferConfig.rx_data = NULL;
   xferConfig.rx_cnt  = 0;
   xferConfig.length  = bufferSize;

   if( spi == SPI0 ) {
      Chip_SSP_RWFrames_Blocking( LPC_SSP1, &xferConfig );
   } else {
      retVal = FALSE;
   }

   return retVal;
}

void Write_Read_Command(spiMap_t spi, uint8_t* wbuffer, uint32_t bufferSize, uint8_t* rbuffer)
{

   bool_t retVal = TRUE;
   Chip_SSP_DATA_SETUP_T xferConfig;

   xferConfig.tx_data = wbuffer;
   xferConfig.tx_cnt  = 0;
   xferConfig.rx_data = rbuffer;
   xferConfig.rx_cnt  = 0;
   xferConfig.length  = bufferSize;

   if( spi == SPI0 ) {
      Chip_SSP_RWFrames_Blocking( LPC_SSP1, &xferConfig );
   } else {
      retVal = FALSE;
   }

   return retVal;
}
