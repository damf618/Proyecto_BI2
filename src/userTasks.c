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
#include "sapi.h"
#include "spi_prim.h"
#include "GPIOINTSETUP.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/
uint8_t datareceived=0;

bool_t TEST_MODE = TEST_PRINCIPAL_STATE ;

debounce_t button4;

dprimario_t prim;

spi_Server_t serv;

//TODO SEND ALLTHIS NEW FEATURES TO THE PRIMARIO LIBRARY
//UART Interrupt Task Handler
TaskHandle_t IntTaskUARTHandle = NULL;
TaskHandle_t IntTaskGPIO1Handle = NULL;
TaskHandle_t IntTaskGPIO2Handle = NULL;
//Semaphore Init
SemaphoreHandle_t xSemaphore = NULL;
//Semaphore SPI Write Init
SemaphoreHandle_t Write_Spi = NULL;
//Semaphore Counter Init
SemaphoreHandle_t Lost_Comm=NULL;
//Mutex UART Init
xSemaphoreHandle gatekeeper=NULL;
//Semaphore Init TEC Primario Logic
SemaphoreHandle_t PrimSemaphore = NULL;
//Semaphore Init  TEC Test Logic
SemaphoreHandle_t TestSemaphore = NULL;
//Queue Init GPIO Init
QueueHandle_t GPIOQueue = NULL;

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/


/*=====[Implementations of public functions]=================================*/

static void Control_Sys( void* taskParmPtr )
{
	/*//DEBUG
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));*/
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 40 ms
	portTickType xPeriodicity =  40 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {
	//MEF Monitor Full Logic
	   primControl(&prim);
	// Send the task to the locked state during xPeriodicity
	// (periodical delay)
	   vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
   }
}

static void State_Test( void* taskParmPtr )
{
	/*//DEBUG
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));*/
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 1000 ms
	portTickType xPeriodicity =  1000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	 // ----- Task repeat for ever -------------------------
	while(TRUE) {
		//Gestion del puerto UART
		if(xSemaphoreTake(gatekeeper,xPeriodicity))
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
		   xSemaphoreGive(gatekeeper);
		}
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}

}

static void CurrentTmode( void* taskParmPtr )
{
  /*//DEBUG
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));*/
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 5000 ms
	portTickType xPeriodicity =  5000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {
		//Gestion del puerto UART
		if(xSemaphoreTake(gatekeeper,xPeriodicity))
		 {

			switch( TEST_MODE ) {
			case TEST_PRINCIPAL_STATE:
				UARTReport( &prim.uart1,"\r\n TEST-MODE: PRINCIPAL STATES \r\n");
				break;
			case TEST_COMM_FLAG:
				UARTReport( &prim.uart1,"\r\n TEST-MODE: UART FLAGS \r\n");
				break;
			}
			xSemaphoreGive(gatekeeper);
		 }
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}

}

static void Test_Mode( void* taskParmPtr )
{
	/*//DEBUG
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));*/
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 100 ms
	portTickType xPeriodicity =  100 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {

		if(get_flag(&button4))
		{											// More info in ButtonCheck
			if(TEST_MODE==TEST_PRINCIPAL_STATE)
				TEST_MODE=TEST_COMM_FLAG;
			else if(TEST_MODE==TEST_COMM_FLAG)
				TEST_MODE=TEST_PRINCIPAL_STATE;
		}

		if (TEST_MODE==TEST_COMM_FLAG){
			if(prim.COMMFLAG==1)					// If there is any transition made for the
				GPIOWrite(TEST_LIGHT,HIGH_G);		// UART console, then the Blue Led turns on.
			else
				GPIOWrite(TEST_LIGHT,LOW_G);
		}
		else if(TEST_MODE==TEST_PRINCIPAL_STATE){
			if((prim.state==PREALARM)||
					(prim.state==PREFAIL)||
					(prim.state==PRENORMAL))		// If we are in PRE-STATE we turn off the
				GPIOWrite(TEST_LIGHT,LOW_G);		// Blue Led.
			else
				GPIOWrite(TEST_LIGHT,HIGH_G);
		}
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

static void Server_SysW( void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	// Tarea periodica cada 10 s
	portTickType xPeriodicity =  3000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE) {
		uint8_t ref=(uint8_t)prim.state;
		SPI_ServerW(&serv,ref);
		//Semaphore to manage the READING from SPI SLAVE
		xSemaphoreGive(Write_Spi);
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

static void Reset_Slave(void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	GPIOOutConfig(Reset_SPI_Slave_pin);				//Configure the SS Pin as Slave
	reset_SPI(&serv,PRE_BOOTING);					//BOOTING Preparations
	vTaskDelay(500);
	reset_SPI(&serv,BOOTING);						//BOOTING SEQUENCE
	vTaskDelay(500);
	reset_SPI(&serv,POST_BOOTING);					//POST BOOTING Config
	char i=0;
	for(i;i<=MAX_COMM_LOST;i++){
		xSemaphoreGive(Lost_Comm);
	}
	vTaskDelete(NULL);

}

static void Server_SysR( void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	bool_t aux=TRUE;
	uint8_t lect[BUFF_SIZE]={0,0,0,0,0,0};
	vTaskDelay(750 / portTICK_RATE_MS);		//Delay to wait for the first SPI writing command
	// Tarea periodica cada 10 s
	portTickType xPeriodicity =  3000 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	while(TRUE)
	{
		//Semaphore to manage the READING from SPI SLAVE
		if(xSemaphoreTake(Write_Spi,xPeriodicity))
		{
			aux=SPI_ServerR(&serv,lect);
		}
		// Mensaje Recibido del Esclavo
		/*//DEBUG
		for(int i=0;i<5;i++){
			printf("Caracter #%d: %d \n",i,lect[i]);
		}
		*/
		//MUTEX, to avoid Priority Inversion, This task has a higher priority
		//compare to the other tasks that makes use of the UART PORT.
		if(xSemaphoreTake(gatekeeper,xPeriodicity))
		{
			if(aux){
				printf("\r\n Good \r\n");
				xSemaphoreGive(Lost_Comm);
			}else
			{
				printf("\r\n Not so good \r\n");
				xSemaphoreGive(gatekeeper);						//For the Last Cycle of the Reset_Slave It needs to release the UART PORT
				if(xSemaphoreTake(  Lost_Comm  , xPeriodicity )==pdTRUE)
				{
					printf("\r\n Available attempts: %d \r\n", uxSemaphoreGetCount(Lost_Comm));

				}else if(uxSemaphoreGetCount(Lost_Comm)==0)
				{
					printf("\r\n Reset Slave \r\n");
					xSemaphoreGive(gatekeeper);					//Release of the Semaphore before moving to Reset
					BaseType_t def= xTaskCreate(
								Reset_Slave,					// Function that implements the task.
								(const char *)"Reset_Slave",	// Text name for the task.
								configMINIMAL_STACK_SIZE,		// Stack size in words, not bytes.
								0,								// Parameter passed into the task.
								tskIDLE_PRIORITY+3,				// Priority at which the task is created.
								0 );							// Pointer to the task created in the system

						if(def==0){
							printf("Error en la creacion de la Tarea Reset_Slave");
						}
				}
			}
		xSemaphoreGive(gatekeeper);
		}
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)

		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

static void onRx(  void *noUsado )
{
	//TODO Anadir QUEUE Para leer y pasarlo como parametro de la Tarea UART
	// Lectura de dato y borrado de flag de interrupcion por UART
	datareceived = uartRxRead( prim.uart1.Uart );

	BaseType_t checkIfYieldRequired = pdFALSE;
	//Verificacion para ceder el CPU a la tarea de interrupcion
	xSemaphoreGiveFromISR( xSemaphore, &checkIfYieldRequired );
	portYIELD_FROM_ISR( checkIfYieldRequired );
}

//PROBABLEMENTE NECESITE UN WRAPPER
/*Hubo que eliminar la definicion de la funcion de Interrupcion GPIO1 en...
 * /home/daniel/Desktop/CIAA/firmware_v3-master/libs/sapi/sapi_v0.5.2/external_peripherals/src/sapi_ultrasonic_hcsr04.c
 */
void PININT_IRQ_HANDLER(void) {

	// Se da aviso que se trato la interrupcion
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	BaseType_t checkIfYieldRequired2= pdFALSE;
	//primUpdates(&prim);
	//Verificacion para ceder el CPU a la tarea de interrupcion
	xSemaphoreGiveFromISR( PrimSemaphore, &checkIfYieldRequired2 );
	portYIELD_FROM_ISR( checkIfYieldRequired2 );

}

//PROBABLEMENTE NECESITE UN WRAPPER
void  GINT0_IRQHandler(void){
	uint8_t aux=0;
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP , 0);
	BaseType_t checkIfYieldRequired1 = pdFALSE;
	//Verificacion para ceder el CPU a la tarea de interrupcion
	xSemaphoreGiveFromISR( PrimSemaphore, &checkIfYieldRequired1 );
	portYIELD_FROM_ISR( checkIfYieldRequired1 );

	//fsmButtonUpdate(&button4);
}

static void IntTaskUART(void* taskParmPtr )
{
	uint8_t aux=0;
	// ----- Task setup -----------------------------------
	// ----- Task repeat for ever -------------------------
	while(1)
	{
		// Has an interrupt ocurred?
		if( xQueueReceive( GPIOQueue,&aux ,portMAX_DELAY ) == pdTRUE )
		{
			UARTUpdate(&prim.uart1, datareceived);
		}
		taskYIELD();
		//No Give because it comes from the Interrupt Handler
		//MOre Info? Check onRx
	}
}

static void IntGPIOPrim(void* taskParmPtr )
{
	uint8_t aux=0;
	while(TRUE) {

		if( xQueueReceive( GPIOQueue,&aux, portMAX_DELAY ) == pdTRUE )
		{
			printf("Got in");
		}
		taskYIELD();
	}
}

static void IntGPIOTest(void* taskParmPtr )
{
	while(TRUE) {

		if( xSemaphoreTake( TestSemaphore, portMAX_DELAY ) == pdTRUE )
		{
			printf("Got in");
		}
		taskYIELD();
	}
}

void Sys_Run( void* taskParmPtr )
{
	/*//DEBUG
	printf("\r\n %s \r\n",pcTaskGetTaskName(NULL));*/
	// ----- Task setup -----------------------------------
	// ----- Semaphore to manage the UART Interrupt Task
	xSemaphore = xSemaphoreCreateBinary();
	if( xSemaphore == NULL )
	{
		printf("Error en la creacion de Semaforo Interrupcion UART");
	}
	// ----- Semaphore to manage the SPI Reading after a write
	Write_Spi = xSemaphoreCreateBinary();
	if( Write_Spi == NULL )
	{
		printf("Error en la creacion de Semaforo Escritura SPI");
	}
	// ----- Semaphore to manage the COMM LOST Protocol
	Lost_Comm = xSemaphoreCreateCounting( MAX_COMM_LOST , INITIAL_COMM_LOST );
	if( Lost_Comm == NULL )
	{
		printf("Error en la creacion de Semaforo Contador Perdida SPI");
	}
	// ----- Semaphore to manage the TEC Primario Logic
	PrimSemaphore = xSemaphoreCreateBinary();
	if( PrimSemaphore == NULL )
	{
		printf("Error en la creacion de Semaforo TEC Primarios");
	}
	// ----- Semaphore to manage the TEC Test Logic
	TestSemaphore = xSemaphoreCreateBinary();
	if( TestSemaphore == NULL )
	{
		printf("Error en la creacion de Semaforo TEC Test");
	}
	// ----- Mutex for managing the UART writing resource
	gatekeeper=xSemaphoreCreateRecursiveMutex();
	if( gatekeeper == NULL )
	{
		printf("Error en la creacion de Mutex Escritura UART");
	}
	/* QUEUE para GPIO*/
	GPIOQueue=xQueueCreate(1,sizeof(uint8_t));
	// SPI configuration
	SPI_INIT(&serv,SPI0, SS_SPI_pin,Reset_SPI_Slave_pin);
	// MEF 1 Init Dispositivo Primario
	primInit(&prim);
	// MEF 2 Init Test
	fsmButtonInit(&button4,TEST_BUTTON);
	// Set of a callback for the event of receiving an UART character
	uartCallbackSet(UART_USB, UART_RECEIVE, onRx, NULL);
	// Enabling UART_USB Interruptions
	uartInterrupt(UART_USB, true);
	// Enabling GPIO Interruptions
	GPIOInt_INIT();

	/*****************Creacion de Tareas*********************/

	BaseType_t res= xTaskCreate(
			Control_Sys,					// Function that implements the task.
			(const char *)"Control_Sys",	// Text name for the task.
			configMINIMAL_STACK_SIZE*2,		// Stack size in words, not bytes.
			0,								// Parameter passed into the task.
			tskIDLE_PRIORITY+2,				// Priority at which the task is created.
			0 );							// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea Control_Sys");
	}

	res= xTaskCreate(
			Test_Mode,						// Function that implements the task.
			(const char *)"Test_Mode",		// Text name for the task.
			configMINIMAL_STACK_SIZE*1,		// Stack size in words, not bytes.
			0,								// Parameter passed into the task.
			tskIDLE_PRIORITY+1,				// Priority at which the task is created.
			0 );							// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea Test_Mode");
	}

	res= xTaskCreate(
			CurrentTmode,					// Function that implements the task.
			(const char *)"CurrentTmode",	// Text name for the task.
			configMINIMAL_STACK_SIZE*1,		// Stack size in words, not bytes.
			0,      						// Parameter passed into the task.
			tskIDLE_PRIORITY+1,         	// Priority at which the task is created.
			0 );                          	// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea CurrentTmode");
	}

	res=xTaskCreate(
	    	State_Test,                     // Function that implements the task.
			(const char *)"State_Test",     // Text name for the task.
			configMINIMAL_STACK_SIZE*2, 	// Stack size in words, not bytes.
			0,                          	// Parameter passed into the task.
			tskIDLE_PRIORITY+1,         	// Priority at which the task is created.
			0 );                    		// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea State_Test");
	}

	res= xTaskCreate(
			IntTaskUART,					// Function that implements the task.
			(const char *)"IntTaskUART",	// Text name for the task.
			configMINIMAL_STACK_SIZE*2, 	// Stack size in words, not bytes.
			0,		    // Parameter passed into the task.
			tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
			IntTaskUARTHandle ); 			// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea IntTaskUART");
		}

	res= xTaskCreate(
			Server_SysW,					// Function that implements the task.
			(const char *)"Server_SysW",	// Text name for the task.
			configMINIMAL_STACK_SIZE*2, 	// Stack size in words, not bytes.
			0,		    // Parameter passed into the task.
			tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
			0 ); 			// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea Server_SysW");
		}

	res= xTaskCreate(
			Server_SysR,					// Function that implements the task.
			(const char *)"Server_SysR",	// Text name for the task.
			configMINIMAL_STACK_SIZE*2, 	// Stack size in words, not bytes.
			0,		    // Parameter passed into the task.
			tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
			0 ); 			// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea Server_SysR");
		}

	res= xTaskCreate(
			IntGPIOPrim,					// Function that implements the task.
			(const char *)"IntGPIOPrim",	// Text name for the task.
			configMINIMAL_STACK_SIZE, 		// Stack size in words, not bytes.
			0,		   						// Parameter passed into the task.
			tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
			IntTaskGPIO1Handle ); 			// Pointer to the task created in the system

	if(res==0){
		printf("Error en la creacion de la Tarea IntGPIOPrim");
		}

	res= xTaskCreate(
			IntGPIOTest,					// Function that implements the task.
			(const char *)"IntGPIOTest",	// Text name for the task.
			configMINIMAL_STACK_SIZE, 		// Stack size in words, not bytes.
			0,		   						// Parameter passed into the task.
			tskIDLE_PRIORITY+2,    			// Priority at which the task is created.
			IntTaskGPIO2Handle ); 			// Pointer to the task created in the system

		if(res==0){
			printf("Error en la creacion de la Tarea IntGPIOTest");
			}


	/*********************************************************/
	// Tarea periodica cada 1 ms
	portTickType xPeriodicity =  40 / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();
	// ----- Task repeat for ever -------------------------
	/*Update Task to Refresh the MEFs related */
	while(TRUE) {


	// Send the task to the locked state during xPeriodicity
	// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

