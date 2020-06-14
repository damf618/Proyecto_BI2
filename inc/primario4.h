/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/12
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __PRIMARIO_H__
#define __PRIMARIO_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include <stdint.h>
#include <stddef.h>
#include <sapi.h>
#include "antirebote_rtos.h"
#include "gpio.h"
#include "Primario_UART.h"
#include "task.h"
#include "FreeRTOS.h"
#include "semphr.h"
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define INITIAL_DEFAULT_STATE NORMAL

#define ALARM_BUTTON TEC1
#define FAIL_BUTTON TEC2
#define NORMAL_BUTTON TEC3
#define TEST_BUTTON TEC4

#define DEF_TIMEOUT 3500				 //Timeout limit between transitions

#define TEST_LIGHT LEDB					// GPIO for the Test LED

#define default_UART_PORT UART_USB

#define SEMAPHORE_ERROR 11
#define SEMAPHOREC_ERROR 22
#define MUTEX_ERROR 33
#define QUEUE_ERROR 44
#define TASK_ERROR 55
#define ALL_GOOD 120

/*=====[Definitions of public data types]====================================*/

// Possible states for the MEFs
typedef enum{ NORMAL , ALARM, FAIL, PRENORMAL, PREALARM, PREFAIL} dprim_state_t;

//Structure with all the associated variables for the Monitor System Logic
typedef struct{
	//Queque Current UART1
	QueueHandle_t intUARTQUEUE;
	//Queque Current STATE
	QueueHandle_t STATEQUEUE;
	//GPIO Interrupt Task Handlers
	TaskHandle_t IntTaskUARTHandle;
	TaskHandle_t IntTaskGPIO1Handle;
	TaskHandle_t IntTaskGPIO2Handle;
	//Queue UART Init
	QueueHandle_t UARTQUEUE;
	//Queue GPIO "GROUP" Init
	QueueHandle_t GPIOQUEUE;
	//Semaphore GPIO Edge Interrupt Handling
	SemaphoreHandle_t GPIO_INTA;
	//Semaphore GPIO Edge Interrupt Handling
	SemaphoreHandle_t GPIO_INTF;
	//Semaphore GPIO Edge Interrupt Handling
	SemaphoreHandle_t GPIO_INTN;
	//Mutex UART Init
	xSemaphoreHandle gatekeeper;
}dprimario_RTOS_Handler_t;

// Structure with the different data types to generate an independent Monitor
typedef struct{
	tick_t timeout;
	delay_t delay;
	dprim_state_t state;
	debounce_t boton1;			//Buttons with  debounce pattern
	debounce_t boton2;
	debounce_t boton3;
	uart_prim_t uart1	;		//Uart interface
	int8_t count;				//Count of Cycles.
	bool_t COMMFLAG;			//Flag for UART interaction 1 ON, 0 OFF
	bool_t TEST_MODE;			//Flag for defining TEST mode
	dprimario_RTOS_Handler_t prim_handler;
}dprimario_t;

/*=====[Prototypes (declarations) of public functions]=======================*/
/** It sets initial conditions for the entire program, it establish the default values
 *  of timeout and #cycles for Alarm and Fail UART code verification.

	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note It must be called before primControl.
	@see primControl.

**/
bool_t primInit(dprimario_t * pPrimario);

/** It identifies the state we are currently and generates the corresponding
 *  outputs verifying if for the current state exists any possible transition.

	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note If the current state is not define it returns FALSE.
	@see primInit.

**/
bool_t primControl(dprimario_t * pPrimario);

/** It updates the state of all the existent MEFs and also checks the TESTMODE.
	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note Must be called after primInit.
	@see primInit.

**/
void primUpdates(dprimario_t * pPrimario);

/** It is used to go directly to the fail state once we come back from an alarm state,
 *  and there is a failure is present in the system.
 * 	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note Must be called after primInit.
	@see primInit.

**/
bool_t Failcheck(dprimario_t * prim);

/** This function creates all the RTOS Tasks the whole system is based on, it returns
 *  0 if there was no problem, in case there was a mistake it will return a value associated to
 *  the RTOS's element that generated the trouble.
	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note TASK_ERROR 55.
	@see primInit.

**/
static char RTOS_Task_Init(dprimario_t * prim);

/** This function creates all the RTOS dependencies the whole system is based on, it returns
 *  0 if there was no problem, in case there was a mistake it will return a value associated to
 *  the RTOS's element that generated the trouble.
	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@note SEMAPHORE_ERROR 11, SEMAPHOREC_ERROR 22, MUTEX_ERROR 33, QUEUE_ERROR 44.
	@see primInit.

**/
static char RTOS_Components_Init(dprimario_t * prim);


//RTOS Verification for primary device
/** This function verifies that all the RTOS elements (Tasks, Semaphores, Mutex, etc.) were
 * created succesfully, otherwise it locks itself on a while(true) loop and prints out the
 * associated error.
 *
	@param pPrimario element of type *dprimario_t* with the Monitor data types needed
	@param status_init used  to evaluate if there was an error during the creation of the RTOS element
	@note SEMAPHORE_ERROR 11, SEMAPHOREC_ERROR 22, MUTEX_ERROR 33, QUEUE_ERROR 44.
	@see primInit specifically RTOS_Components_Init and RTOS_Task_Init.

**/
bool_t RTOS_Check(dprimario_t * pPrimario,char status_init);

/*==================[RTOS TASKS]============================================*/

/** Program associated for the UART received interrupt, it stores the received character and
 * releases the CPU and yields it to the IntTaskUART, to the processingofthe info.
 *
 * Task Name: "OnRx"
 * Priority: +2
 * Parameter: None
 * Trigger: UART character received

**/
static void onRx(  void *taskParmPtr);

/** Task to be executed after the event of an UART interrupt, this task must be executed after
 * to process the received data and run the update of the MEF responsible for the UART Command
 * refresh.
 *
 *
 * Task Name: "IntTaskUART"
 * Priority: +2
 * Parameter: None
 * Trigger: UART character received

	@note This Task is created by the Sys_Run function.
	@see OnRx
**/
static void IntTaskUART(void* taskParmPtr );

/** Task to be executed after the event of an GPIO interrupt, this task must be executed after the interrupt
 * to properly manage the event of pressing o releasing a pushbutton. It receives which was the pin who
 * generated the interrupt and acts accordingly.
 *
 *
 *
 * Task Name: "IntTaskUART"
 * Priority: +2
 * Parameter: dprimario_t variable
 * Trigger: Change of state an of the TEC1, TEC2 and TEC3 pins

	@note This Task is created by the Sys_Run function.
	@see OnRx
**/
static void IntGPIOPrim(void* taskParmPtr );

/** One Shot Task to be executed only when  we are coming back from an alarm state, it verifies if there is
 *  a trouble state present and it automatically set the FAIL state without the pre-fail state in
 *   consideration.
 *
 *
 *
 * Task Name: "MaskedFail"
 * Priority: +5
 * Parameter: dprimario_t variable
 * Trigger: Change of state an of the TEC1, TEC2 and TEC3 pins

	@note This Task is created by the Sys_Run function.
	@see OnRx
**/
static void MaskedFail(void* taskParmPtr);
/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __PRIMARIO_H__ */
