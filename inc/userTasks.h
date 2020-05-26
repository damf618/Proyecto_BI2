/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __USER_TASKS_H__
#define __USER_TASKS_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "sapi.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/
#define MAX_COMM_LOST 3			//Used for the Semaphore in charge of the Slave Reset
#define INITIAL_COMM_LOST 3		//Used for the Semaphore in charge of the Slave Reset

#define TEST_PRINCIPAL_STATE 0
#define TEST_COMM_FLAG 1

# define Reset_SPI_Slave_pin GPIO5
/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/
/** It sets initial conditions, creates the different tasks to execute properly
 *  the Monitor System it must be called to start the entire system. If all the
 *  tasks and RTOS tools were created correctly it is called periodically to
 *  update the relevant MEFS of the system.
 * Task Name: "Sys_Run"
 * Priority: +1
 * Parameter: None
 * Trigger: User

	@note The system will execute the whole system based on the default parameters,
	Any modification can usually be made from the .h files.


**/
void Sys_Run( void* taskParmPtr );  // Task declaration

/*=====[Prototypes (declarations) of private functions]=============*/

/** Task related to the entire logic of the system, it is structured as a MEF, it requires
 * the call of the update function periodically.
 * Task Name: "Control_Sys"
 * Priority: +2
 * Parameter: None
 * Trigger: Periodicity, 40ms

	@note There is no Task assigned specifically to for the Update Task, it is a shared
	responsibility of the Sys_Run task after it creates the tasks it is the responsible.
	@see Sys_Run
**/
static void Control_Sys( void* taskParmPtr );

/** Task for Debug/Maintenance purposes. It allows the user to verify the current state
 * through the UART Console.
 * Task Name: "State_Test"
 * Priority: +1
 * Parameter: None
 * Trigger: Periodicity, 1000ms

	@note This Task is created by the Sys_Run function.
	@see Sys_Run
**/
static void State_Test( void* taskParmPtr );

/** Task for Debug/Maintenance purposes. It allows the user to verify which is the current
 *  test mode and it reports it to the user through UART Console.
 *
 * Task Name: "CurrenTmode"
 * Priority: +1
 * Parameter: None
 * Trigger: Periodicity, 5000ms

	@note This Task is created by the Sys_Run function..
	@see CurrenTmode , Sys_Run
**/
static void CurrentTmode( void* taskParmPtr );

/** Task for Debug/Maintenance purposes. This function verifies if the Test Button was pressed
 * and if it was pressed toggles between test modes. By default it starts with the
 * PRINCIPAL_STATE_MODE. It allows the user to verify the current test mode selected, there are
 *  2 modes. The different modes affects why does the TEST LED (BLUE LED by default) turns on/off.
 *
 *  PRINCIPAL_STATE MODE: It turns on the Test LED if there was a transition caused by  current
 *   state is one of the following states: NORMAL, FAIL, ALARM.
 *
 *  TEST_COMM_FLAG MODE: Now the Test LED will turn on if a change of state was triggered by an
 *   UART Command, every time a UART Command is received the Test LED will be turned on.
 *
 * Task Name: "Test_Mode"
 * Priority: +1
 * Parameter: None
 * Trigger: Periodicity, 100ms

	@note This Task is created by the Sys_Run function.
	@see CurrenTmode ,
**/
static void Test_Mode( void* taskParmPtr );

/** Task to establish communication through SPI protocol to the NodeMCU to post the current
 * state of the system to the firebase realtime database. The entire task is executed in 2
 * cycles the first cycle consist in the writing protocol, where the EDU-CIAA acts as master
 * and sends the Writing Command + current state, in the next call of the function the 2nd
 * cycle starts which consist in reading the response of the NodeMCU related to the execution
 * of the update of the server current state.
 *
 * Task Name: "Server_Sys"
 * Priority: +2
 * Parameter: None
 * Trigger: Periodicity, 10s

	@note This Task is created by the Sys_Run function..
	@see Sys_Run
**/
static void Server_SysW( void* taskParmPtr );


static void Server_SysR( void* taskParmPtr );

static void Reset_Slave(void* taskParmPtr );

/** Program associated for the UART received interrupt, it stores the received character and
 * releases the CPU and yields it to the IntTaskUART, to the processingofthe info.
 *
 * Task Name: "OnRx"
 * Priority: +2
 * Parameter: None
 * Trigger: UART character received

**/
static void onRx(  void *noUsado );

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

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* __USER_TASKS_H__ */
