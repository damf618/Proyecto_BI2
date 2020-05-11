/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "userTasks.h"

/*=====[Inclusions of private function dependencies]=========================*/

#include "antirebote_rtos.h"
#include "gpio.h"
#include "primario4.h"
#include "task.h"
#include "FreeRTOS.h"
#include "semphr.h"

/*=====[Definition macros of private constants]==============================*/

#define TEST_BUTTON TEC4
#define TEST_PRINCIPAL_STATE 0
#define TEST_COMM_FLAG 1

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/
xSemaphoreHandle gatekeeper=0;
/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

debounce_t button1;
dprimario_t prim;
debounce_t button4;
bool_t TEST_MODE = TEST_PRINCIPAL_STATE ;


/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

// Task implementation

void Init_Sys( void* taskParmPtr )
{
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
   // ----- Task setup -----------------------------------
   // ----- Task repeat for oneshot -------------------------
   primInit(&prim);
   fsmButtonInit(&button4,TEST_BUTTON);
   gatekeeper=xSemaphoreCreateRecursiveMutex();
   vTaskDelete(NULL);
}

void Update_Sys( void* taskParmPtr )
{
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
   // ----- Task setup -----------------------------------
	// Tarea periodica cada 1 ms
	portTickType xPeriodicity =  1 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
   // ----- Task repeat for ever -------------------------
   while(TRUE) {
	   if(xSemaphoreTake(gatekeeper,1000))
	   {
		   primUpdates(&prim);
		   xSemaphoreGive(gatekeeper);
	   }
	   fsmButtonUpdate(&button4);
      // Send the task to the locked state during xPeriodicity
      // (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
   }
}

void Control_Sys( void* taskParmPtr )
{
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 40 ms
		portTickType xPeriodicity =  40 / portTICK_RATE_MS;
		portTickType xLastWakeTime = xTaskGetTickCount();
	   // ----- Task repeat for ever -------------------------
	   while(TRUE) {
		   primControl(&prim);
		  // Send the task to the locked state during xPeriodicity
	      // (periodical delay)
			vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	   }
}

void State_Test( void* taskParmPtr )
{
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 1000 ms
	portTickType xPeriodicity =  1000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	 // ----- Task repeat for ever -------------------------
	while(TRUE) {
		if(xSemaphoreTake(gatekeeper,1000))
		{
			switch( prim.state )
			{
			case PRENORMAL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PRE-NORMAL \r\n");
				//printf("\r\n CURRENT STATE: PRE-NORMAL \r\n");
				break;
			case PREALARM:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PRE-ALARM \r\n");
				//printf("\r\n CURRENT STATE: PRE-ALARM \r\n");
				break;
			case PREFAIL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PRE-FAIL \r\n");
				//printf("\r\n CURRENT STATE: PRE-FAIL \r\n");
				break;
			case NORMAL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: NORMAL \r\n");
				//printf("\r\n CURRENT STATE: NORMAL \r\n");
				break;
			case FAIL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: FAIL\r\n");
				//printf("\r\n CURRENT STATE: FAIL \r\n");
				break;
			case ALARM:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: ALARM\r\n");
				//printf("\r\n CURRENT STATE: ALARM \r\n");
				break;
			default:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PLEASE RESTART\r\n");
				//printf("\r\n CURRENT STATE: PLEASE RESTART \r\n");
				break;
			}
		   xSemaphoreGive(gatekeeper);
		}
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}

}

void CurrentTmode( void* taskParmPtr )
{
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 5000 ms
	portTickType xPeriodicity =  5000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {
		if(xSemaphoreTake(gatekeeper,1000))
		 {

			switch( TEST_MODE ) {
			case TEST_PRINCIPAL_STATE:
				UARTReport( &prim.uart1,"\r\n TEST-MODE: PRINCIPAL STATES \r\n");
				//printf("\r\n TEST-MODE: PRINCIPAL STATES \r\n");
				break;
			case TEST_COMM_FLAG:
				UARTReport( &prim.uart1,"\r\n TEST-MODE: UART FLAGS \r\n");
				//printf("\r\n TEST-MODE: UART FLAGS \r\n");
				break;
			}
			xSemaphoreGive(gatekeeper);
		 }
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}

}

void Test_Mode( void* taskParmPtr )
{
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 100 ms
	portTickType xPeriodicity =  100 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {

		if(get_flag(&button4))
		{			// More info in ButtonCheck
			if(TEST_MODE==TEST_PRINCIPAL_STATE)
				TEST_MODE=TEST_COMM_FLAG;
			else if(TEST_MODE==TEST_COMM_FLAG)
				TEST_MODE=TEST_PRINCIPAL_STATE;
		}

		if (TEST_MODE==TEST_COMM_FLAG){
			if(prim.COMMFLAG==1)		// If there is any transition made for the
				GPIOWrite(TEST_LIGHT,HIGH_G);		// UART console, then the Blue Led turns on.
			else
				GPIOWrite(TEST_LIGHT,LOW_G);
		}
		else if(TEST_MODE==TEST_PRINCIPAL_STATE){
			if((prim.state==PREALARM)||
					(prim.state==PREFAIL)||
					(prim.state==PRENORMAL))	// If we are in PRE-STATE we turn off the
				GPIOWrite(TEST_LIGHT,LOW_G);	// Blue Led.
			else
				GPIOWrite(TEST_LIGHT,HIGH_G);
		}
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );

	}
}


/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

