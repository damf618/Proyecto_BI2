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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "sapi.h"
#include "spi_prim.h"
#include "GPIOINTSETUP.h"
#include "chip.h"

/*=====[Definitions of private data types]===================================*/
/*Test and SPI*/
bool_t TEST_MODE = TEST_PRINCIPAL_STATE ;
debounce_t button4;
spi_Server_t serv;

/*MEF Struct for the System Monitor Logic*/
dprimario_t prim;


//Semaphore SPI Write Init
SemaphoreHandle_t Write_Spi = NULL;
//Semaphore Init  TEC Test Logic
SemaphoreHandle_t TestSemaphore = NULL;
//Semaphore Counter Init
SemaphoreHandle_t Lost_Comm=NULL;
//Interrupt Task related Hanlder
TaskHandle_t IntTaskGPIOTestHandle=NULL;


//	Test Button Interrupt Hanlder
void TEST_IRQ_HANDLER(void) {

	// Limpiar bandera de interrupcion
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	BaseType_t gpioupdate= pdFALSE;
	//Verificacion para ceder el CPU a la tarea de interrupcion
	xSemaphoreGiveFromISR( TestSemaphore, &gpioupdate );
	portYIELD_FROM_ISR( gpioupdate );

}

//Periodic task to report the current state
static void State_Test( void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 1000 ms
	portTickType xPeriodicity =  1000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	 // ----- Task repeat for ever -------------------------
	while(TRUE) {
		//Gestion del puerto UART
		if(xSemaphoreTake(prim.prim_handler.gatekeeper,xPeriodicity))
		{
			switch( prim.state )
			{
			case PRENORMAL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PRE-NORMAL \r\n");
				break;
			case PREALARM:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PRE-ALARM \r\n");
				break;
			case PREFAIL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PRE-FAIL \r\n");
				break;
			case NORMAL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: NORMAL \r\n");
				break;
			case FAIL:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: FAIL\r\n");
				break;
			case ALARM:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: ALARM\r\n");
				break;
			default:
				UARTReport( &prim.uart1,"\r\n CURRENT STATE: PLEASE RESTART\r\n");
				break;
			}
		   xSemaphoreGive(prim.prim_handler.gatekeeper);
		}
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}

}

//Periodic task toreport the current test mode
static void CurrentTmode( void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 5 s
	portTickType xPeriodicity =  5000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {
		//Gestion del puerto UART
		if(xSemaphoreTake(prim.prim_handler.gatekeeper,xPeriodicity))
		 {
			switch( TEST_MODE ) {
			case TEST_PRINCIPAL_STATE:
				UARTReport( &prim.uart1,"\r\n TEST-MODE: PRINCIPAL STATES \r\n");
				break;
			case TEST_COMM_FLAG:
				UARTReport( &prim.uart1,"\r\n TEST-MODE: UART FLAGS \r\n");
				break;
			}
			xSemaphoreGive(prim.prim_handler.gatekeeper);
		 }
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}

}

//Blocked task, executed once the test interrupt handler allows it
static void IntGPIOTest(void* taskParmPtr )
{

	while(TRUE) {

		xSemaphoreTake( TestSemaphore, portMAX_DELAY );
		//Debounce
		NVIC_DisableIRQ(SINGLE);
		vTaskDelay( DEBOUNCE_TIME / portTICK_RATE_MS);
		NVIC_EnableIRQ(SINGLE);
		//Toggle Between Test States
		if(TEST_MODE==TEST_PRINCIPAL_STATE)
			TEST_MODE=TEST_COMM_FLAG;
		else if(TEST_MODE==TEST_COMM_FLAG)
			TEST_MODE=TEST_PRINCIPAL_STATE;
	}
}

//Periodic task to control the test led
static void Test_Mode( void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 100 ms
	portTickType xPeriodicity =  100 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {

		if (TEST_MODE==TEST_COMM_FLAG){
			if(prim.COMMFLAG==1)					// If there were a request for transition made by the
				GPIOWrite(TEST_LIGHT,HIGH_G);		// UART console, then the Blue Led turns on.
			else
				GPIOWrite(TEST_LIGHT,LOW_G);
		}
		else if(TEST_MODE==TEST_PRINCIPAL_STATE){
			if((prim.state==PREALARM)||
					(prim.state==PREFAIL)||
					(prim.state==PRENORMAL))		// If we are in PRE-STATE we turn off the
				GPIOWrite(TEST_LIGHT,LOW_G);		// Blue Led, else turnthe led on.
			else
				GPIOWrite(TEST_LIGHT,HIGH_G);
		}

		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

//Periodic task to write to the SPI Slave
static void Server_SysW( void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 3 s
	portTickType xPeriodicity =  3000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {
		uint8_t ref=(uint8_t)prim.state;
		SPI_ServerW(&serv,ref);
		//Semaphore to manage the READING from SPI SLAVE
		xSemaphoreGive(Write_Spi);
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

/*Oneshot task to reset the SPI Slave and manage the correct pin
sequence to allow the booting of the NodeMCU*/
static void Reset_Slave(void* taskParmPtr )
{
	// ----- One Shot Task

	GPIOOutConfig(Reset_SPI_Slave_pin);				//Configure the SS Pin as Slave
	reset_SPI(&serv,PRE_BOOTING);					//BOOTING Preparations
	vTaskDelay(250);
	reset_SPI(&serv,BOOTING);						//BOOTING SEQUENCE
	vTaskDelay(250);
	reset_SPI(&serv,POST_BOOTING);					//POST BOOTING Config
	int8_t i;
	for(i=0;i<=MAX_COMM_LOST;i++){
		xSemaphoreGive(Lost_Comm);
	}
	vTaskDelete(NULL);

}

/*Periodic task it is executed once the SPIWrite was made,it also
creates the Reset_Slave tasks if needed*/
static void Server_SysR( void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	char status=ALL_GOOD;
	bool_t aux=TRUE;
	uint8_t lect[BUFF_SIZE]={0,0,0,0,0,0};
	vTaskDelay(SPI_WRITE_READ_DELAY / portTICK_RATE_MS);		//Delay to wait for the first SPI writing command
	// Tarea periodica cada 3 s
	portTickType xPeriodicity =  3000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE)
	{
		//Semaphore to manage the READING from SPI SLAVE
		if(xSemaphoreTake(Write_Spi,xPeriodicity)&&(uxSemaphoreGetCount(Lost_Comm)!=0))
		{
			aux=SPI_ServerR(&serv,lect);
		}

		//MUTEX, to avoid Priority Inversion
		if(xSemaphoreTake(prim.prim_handler.gatekeeper,xPeriodicity))
		{
			if(aux){
				UARTReport( &prim.uart1,"\r\n Good \r\n");
				xSemaphoreGive(Lost_Comm);
			}else
			{
				UARTReport( &prim.uart1,"\r\n  Not so good \r\n");
				if(xSemaphoreTake(  Lost_Comm  , 0 )==pdTRUE)
				{
					printf("\r\n Available attempts: %d \r\n", uxSemaphoreGetCount(Lost_Comm));

				}else if(uxSemaphoreGetCount(Lost_Comm)==0)
				{
					UARTReport( &prim.uart1,"\r\n Reset Slave \r\n");
					xSemaphoreGive(prim.prim_handler.gatekeeper);	//Release of the Semaphore before moving to Reset
					BaseType_t def= xTaskCreate(
								Reset_Slave,					// Function that implements the task.
								(const char *)"Reset_Slave",	// Text name for the task.
								configMINIMAL_STACK_SIZE,		// Stack size in words, not bytes.
								0,								// Parameter passed into the task.
								tskIDLE_PRIORITY+3,				// Priority at which the task is created.
								0 );							// Pointer to the task created in the system

					if(def==0){
						status=TASK_ERROR;
					}
					RTOS_Check(&prim,status);
				}
			}
		xSemaphoreGive(prim.prim_handler.gatekeeper);
		}
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

/*Periodic Task, inits the entiresystem and updates the Monitor
MEFs logic */
void Sys_Run( void* taskParmPtr )
{
	char status=ALL_GOOD;
	// ----- Semaphore to manage the SPI Reading after a write
	Write_Spi = xSemaphoreCreateBinary();
	if( Write_Spi == NULL )
		status=SEMAPHORE_ERROR;

	// ----- Semaphore to manage the COMM LOST Protocol
	Lost_Comm = xSemaphoreCreateCounting( MAX_COMM_LOST , INITIAL_COMM_LOST );
	if( Lost_Comm == NULL )
		status=SEMAPHOREC_ERROR;

	// ----- Semaphore to manage the TEC Test Logic
	TestSemaphore = xSemaphoreCreateBinary();
	if( TestSemaphore == NULL )
		status=SEMAPHORE_ERROR;

	//RTOS Components Verification
	RTOS_Check(&prim,status);

	// ----- Task setup -----------------------------------
	// Full Init RTOS and Monitor MEF
	primInit(&prim);
	// SPI configuration
	SPI_INIT(&serv,SPI0, SS_SPI_pin,Reset_SPI_Slave_pin);
	// MEF 2 Init Test
	fsmButtonInit(&button4,TEST_BUTTON);
	// Test Interrupt Configuration
	GPIOTestInt_INIT();

	/*********************************************************/

	BaseType_t res= xTaskCreate(
			Server_SysW,					// Function that implements the task.
			(const char *)"Server_SysW",	// Text name for the task.
			configMINIMAL_STACK_SIZE*2, 	// Stack size in words, not bytes.
			0,		 					    // Parameter passed into the task.
			tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
			0 ); 							// Pointer to the task created in the system

	if(res==0){
		status= TASK_ERROR;
	}

	res= xTaskCreate(
			Server_SysR,					// Function that implements the task.
			(const char *)"Server_SysR",	// Text name for the task.
			configMINIMAL_STACK_SIZE*2, 	// Stack size in words, not bytes.
			0,		    					// Parameter passed into the task.
			tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
			0 ); 							// Pointer to the task created in the system

	if(res==0){
		status= TASK_ERROR;
	}

	res= xTaskCreate(
			Test_Mode,						// Function that implements the task.
			(const char *)"Test_Mode",		// Text name for the task.
			configMINIMAL_STACK_SIZE*1,		// Stack size in words, not bytes.
			0,								// Parameter passed into the task.
			tskIDLE_PRIORITY+1,				// Priority at which the task is created.
			0 );							// Pointer to the task created in the system

	if(res==0){
		status= TASK_ERROR;
	}

	res= xTaskCreate(
			CurrentTmode,					// Function that implements the task.
			(const char *)"CurrentTmode",	// Text name for the task.
			configMINIMAL_STACK_SIZE*1,		// Stack size in words, not bytes.
			0,      						// Parameter passed into the task.
			tskIDLE_PRIORITY+1,         	// Priority at which the task is created.
			0 );                          	// Pointer to the task created in the system

	if(res==0){
		status= TASK_ERROR;
	}

	res=xTaskCreate(
			State_Test,                     // Function that implements the task.
			(const char *)"State_Test",     // Text name for the task.
			configMINIMAL_STACK_SIZE*2, 	// Stack size in words, not bytes.
			0,                          	// Parameter passed into the task.
			tskIDLE_PRIORITY+1,         	// Priority at which the task is created.
			0 );                    		// Pointer to the task created in the system

	if(res==0){
		status= TASK_ERROR;
	}

	res= xTaskCreate(
				IntGPIOTest,					// Function that implements the task.
				(const char *)"IntGPIOTest",	// Text name for the task.
				configMINIMAL_STACK_SIZE, 		// Stack size in words, not bytes.
				0,		   						// Parameter passed into the task.
				tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
				IntTaskGPIOTestHandle); 			// Pointer to the task created in the system

		if(res==0){
			status= TASK_ERROR;
		}


	//RTOS Task Verification
	RTOS_Check(&prim,status);

	/*********************************************************/
	// Tarea periodica cada 40 ms
	portTickType xPeriodicity =  40 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	/*Update Task to Refresh the MEFs related */
	while(TRUE) {
		//MEF Monitor Full Logic
		primControl(&prim);

		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

