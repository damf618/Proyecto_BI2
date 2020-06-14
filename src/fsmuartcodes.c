/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/12
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "Primario_UART.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*=====[Declaration of Functions]============================================*/

waitForReceiveStringOrTimeoutState_t waitForReceiveStringOrTimeout2(
		uartMap_t uart,waitForReceiveStringOrTimeout_t* instance , char receiveByte,uart_prim_t * uprim)
{

   switch( instance->state ) {

   case UART_RECEIVE_STRING_CONFIG:

	   uprim->InitTick=xTaskGetTickCount();

	   instance->stringIndex = 0;

	   instance->state = UART_RECEIVE_STRING_RECEIVING;

	   break;

   case UART_RECEIVE_STRING_RECEIVING:

	   if( xSemaphoreTake(  uprim->Msg_Timeout ,0 )==pdTRUE){
		   instance->state = UART_RECEIVE_STRING_TIMEOUT;
	   }
	   else{

		   if( (instance->string)[(instance->stringIndex)] == receiveByte ) {

			   (instance->stringIndex)++;

			   if( (instance->stringIndex) == (instance->stringSize - 1) ) {
				   instance->state = UART_RECEIVE_STRING_RECEIVED_OK;
			   }

		   }
	   }
      break;

   case UART_RECEIVE_STRING_RECEIVED_OK:
      instance->state = UART_RECEIVE_STRING_CONFIG;
      break;

   case UART_RECEIVE_STRING_TIMEOUT:
      instance->state = UART_RECEIVE_STRING_CONFIG;
      break;

   default:
      instance->state = UART_RECEIVE_STRING_CONFIG;
      break;
   }

   return instance->state;
}

