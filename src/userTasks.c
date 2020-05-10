/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"
#include "antirebote_rtos.h"
#include "gpio.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

debounce_t button1;

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

// Task implementation
void myTask( void* taskParmPtr )
{
   // ----- Task setup -----------------------------------
   printf( "Blinky with freeRTOS y sAPI.\r\n" );

   GPIOWrite( LEDB, ON );

   // Send the task to the locked state for 1 s (delay)
   vTaskDelay( 1000 / portTICK_RATE_MS );

   GPIOWrite( LEDB, OFF );

   // Periodic task every 500 ms
   portTickType xPeriodicity =  500 / portTICK_RATE_MS;
   portTickType xLastWakeTime = xTaskGetTickCount();

   // ----- Task repeat for ever -------------------------
   while(TRUE) {
	   GPIOToggle( LED1 );
	   if(get_flag(&button1))
	   {
		   printf( "Blink!\r\n" );
	   	   printf("Duracion del boton presionado: %d ms\r\n",button1.pressed_time);
	   }
	   vTaskDelay( 40 / portTICK_RATE_MS );
      // Send the task to the locked state during xPeriodicity
      // (periodical delay)
      vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
   }
}

void myTask2( void* taskParmPtr )
{
   // ----- Task setup -----------------------------------
	fsmButtonInit(&button1,TEC4);
   // ----- Task repeat for ever -------------------------
   while(TRUE) {
	   fsmButtonUpdate(&button1);
	   vTaskDelay( 1 / portTICK_RATE_MS );
      // Send the task to the locked state during xPeriodicity
      // (periodical delay)
   }
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

