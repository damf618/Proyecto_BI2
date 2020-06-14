/*
 * Primario_UART.h
 *
 *  Created on: Apr 12, 2020
 *      Author: daniel
 */

#ifndef PROGRAMAS_DAMF_PRIMARIO_INC_PRIMARIO_UART_H_
#define PROGRAMAS_DAMF_PRIMARIO_INC_PRIMARIO_UART_H_

#include <sapi.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define UARTMSGLONG 100

#define ALARMMSG "al987"
#define FAILMSG "fa123"
#define NORMALMSG "no456"

#define NCodes 3 				 // number of codes to be monitored (ALARMMSG, FAILMSG and NORMALMSG)
#define CYCLES 18				 // should be a multiple of NCodes, it represents how many times
				 	 	 	 	 // it will check for each of the different UART Modes in the timeout time

#define BAUD_RATE 115200		 //BAUD RATE for the Uart Port

typedef enum{ FAILURES, ALARMS, NORMALS} uart_mode_t;	// What are we monitoring Fail codes
														// or Alarm codes.

// Structure with all the data needed to send an UART Message
typedef struct{
	char dato_tx[UARTMSGLONG]; 		//Bloque de memoria de la transmision en curso
	uint8_t txCounter; 				//contador de bytes transmitidos
	uint8_t txLen; 					//longitud del paquete en transmision
}tx_message;

// Structure with the different data types to generate an UART Listening Process
typedef struct{
	uartMap_t Uart;
	waitForReceiveStringOrTimeout_t waitText;
	waitForReceiveStringOrTimeoutState_t waitTextState;
	bool_t type;
	uart_mode_t mode;
	SemaphoreHandle_t Msg_Timeout;
	TickType_t InitTick;
	TickType_t CurrentTick;
	QueueHandle_t onTxQueue;
	tx_message messageTX;
}uart_prim_t;

//Possible results of the UART Listening Process
typedef enum{ UART_timeout, UART_received, UART_reading} uart_state_t;

/** It sets initial conditions for the UART Listening Process, it is based on a
 *  MEF created by Eric Pernia, specifically the sapi UART example "receive string".
 *  It receives bytes and compares then with a previously established code, during
 *  a period of time, if the time runs out the timeout flags turns on. If the code
 *  was received before the time runs out, the code received flag turns on.

	@param uprim element of type *uart_prim_t* with the uart related data types needed
	@param code it is used to specify which code are we listening to. Alarm or Fail.
	@param Uart it is used to specify the UART Port to be used
	@param timeout The amount of time available before the timeout flag turns on.
	@note It must be called before every process that involves the UART Port.

**/
bool_t UARTInit(uart_prim_t * uprim,bool_t code,uartMap_t Uart,tick_t timeout);

/** It returns the current state of the UART Listening process. It must be called
 *  after the "Update" (UARTUpdate) to work correctly.

	@param uprim element of type *uart_prim_t* with the uart related data types needed
	@note It returns UART_timeout (0) or UART_received (1).
	@see UARTUpdate.

**/
char UARTRead(uart_prim_t * uprim);

/** It resets the UART Listening Process, setting the listening state in the internal
 *  MEF and restarts the timeout delay.

	@param uprim element of type *uart_prim_t* with the uart related data types needed

**/
void UARTReset(uart_prim_t * uprim);

/** It is the responsible to update the internal UART Listening Process MEF, it
 *  verify that neither the timeout or the received event have occurred, before
 *  every iteration, it receive bytes through the specified UART Port and compares
 *  checks if all the characters present in the listening code were received.

	@param uprim element of type *uart_prim_t* with the uart related data types needed
	@note It must be called after the UARTInit to work correctly.
	@see UARTInit.

**/
void UARTUpdate(uart_prim_t * uprim, char cdata);

/** To listen different codes in one UART Port, we need to monitor one code and after
 *  that monitor a different code, we can not monitor 2 codes at the same time,
 *  this function is used to change the code we are currently listening to.

	@param uprim element of type *uart_prim_t* with the uart related data types needed
	@param code  it set which of the codes are we listening to, Alarm or Fail code.
	@note It does not reset the UART Listening Process.

**/
void UARTChange(uart_prim_t * uprim,bool_t code);

/** To write a message through the UART Port selected .

	@param uprim element of type *uart_prim_t* with the uart related data types needed
	@param *text  message to be written.
	@note Based on sapi library.

**/
void UARTReport(uart_prim_t * uprim, char *text);

/** Adaptation of the original code to be FreeRTOS compatible, this function controls
 * the timeout event and gives the corresponding semaphore.
 *
 *
 	@param uprim element of type *uart_prim_t* with the uart related data types needed
	@param cdata  received byte from the uart interrupt.
	@note Based on sapi library.
 *
 *
 **/
void Time_Update(uart_prim_t * uprim, char cdata);

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif


#endif /* PROGRAMAS_DAMF_PRIMARIO_INC_PRIMARIO_UART_H_ */
