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

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/



/*********************************************/
waitForReceiveStringOrTimeoutState_t waitForReceiveStringOrTimeout2(
   uartMap_t uart, waitForReceiveStringOrTimeout_t* instance , char receiveByte)
{

   switch( instance->state ) {

   case UART_RECEIVE_STRING_CONFIG:

      delayInit( &(instance->delay), instance->timeout );

      instance->stringIndex = 0;

      instance->state = UART_RECEIVE_STRING_RECEIVING;

      break;

   case UART_RECEIVE_STRING_RECEIVING:

	   if( (instance->string)[(instance->stringIndex)] == receiveByte ) {

		   (instance->stringIndex)++;

		   if( (instance->stringIndex) == (instance->stringSize - 1) ) {
			   instance->state = UART_RECEIVE_STRING_RECEIVED_OK;
		   }

	   }

      if( delayRead( &(instance->delay) ) ) {
         instance->state = UART_RECEIVE_STRING_TIMEOUT;
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
/*******************************************************************/
