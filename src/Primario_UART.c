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
#include "fsmuartcodes.h"


int window=0;	// For the calculations of the time needed to fulfill the number of
				// cycles specified

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

// It resets the UART Listening process and also the timeout delay per transition
void UARTReset(uart_prim_t * uprim){
	uprim->waitTextState = UART_RECEIVE_STRING_CONFIG;	// It resets the MEF stage to receive this
    uprim->waitText.state = UART_RECEIVE_STRING_CONFIG; // way we can init the UART Listening Process
    delayInit(&uprim->waitText.delay,window);			// Configure the UART Listening Process delay for the timeout measuring.
}

// It is designed to be used in those cases where more than one code is being
// monitored on the same UART port
void UARTChange (uart_prim_t * uprim,bool_t code){
	if(code==ALARMS){							// If we want to monitor alarms, we need to
		uprim->waitText.string = ALARMMSG;		// wait for the Alarm code, and change the mode
		uprim->mode=ALARMS;						// notify the user.
	}
	else if(code==FAILURES){					// If we want to monitor failures, we need to
		uprim->waitText.string =  FAILMSG;		// wait for the Fail code, and change the mode
		uprim->mode=FAILURES;					// notify the user.
	}
	else if(code==NORMALS){						// If we want to monitor normal, we need to
			uprim->waitText.string =  NORMALMSG;// wait for the Normal code, and change the mode
			uprim->mode=NORMALS;					// notify the user.
	}
	uprim->waitTextState = UART_RECEIVE_STRING_CONFIG;
	uprim->waitText.state = UART_RECEIVE_STRING_CONFIG;
	uprim->waitText.stringSize = sizeof(uprim->waitText.string); // We must set the new size of the code we are waiting.
}

// It sets the parameters for the UART configuration, sets the codes for the
// UART transition and initialize the UART Listening process with the calculated
// window
bool_t UARTInit(uart_prim_t * uprim,bool_t code,uartMap_t Uart,tick_t timeout){
	uprim->Uart =Uart;
	uartConfig( uprim->Uart , BAUD_RATE );		//UART Port and Baud Rate Configuration
	if(code==ALARMS){						//Info in UARTChange
		uprim->waitText.string = ALARMMSG;
		uprim->mode=ALARMS;
	}
	else if(code==FAILURES){
		uprim->waitText.string =  FAILMSG;
		uprim->mode=FAILURES;
	}
	else if(code==NORMALS){
			uprim->waitText.string =  NORMALMSG;
			uprim->mode=NORMALS;
	}
	uprim->waitTextState = UART_RECEIVE_STRING_CONFIG;
	uprim->waitText.state = UART_RECEIVE_STRING_CONFIG;
    uprim->waitText.stringSize = sizeof(uprim->waitText.string); // Sets the size of the code we are waiting
    window=(int)(timeout/(CYCLES/NCodes));			// lapse of time needed to accomplish the # cycles established by the user
    uprim->waitText.timeout = window;				// set the time for the waiting time of each code.
    return 1;
}

// It updates the state of the MEFS, Machine in charge of the UART Listening
// Process to verify if there is any possible transition
void UARTUpdate(uart_prim_t * uprim, char cdata){
	if( uprim->waitTextState != UART_RECEIVE_STRING_RECEIVED_OK &&  //Asks if none of the timeout or received message events were reached.
		             uprim->waitTextState != UART_RECEIVE_STRING_TIMEOUT )
		uprim->waitTextState = waitForReceiveStringOrTimeout2( uprim->Uart , &uprim->waitText, cdata ); //Updates the MEF to receive the next byte.
}

// It reads the MEFS of the UART Listening Process to verify if there is
// any of the 2 possible events
char UARTRead(uart_prim_t * uprim){
		if( uprim->waitTextState == UART_RECEIVE_STRING_TIMEOUT ){  // Asks for the timeout flag. Must be called after
			return UART_timeout;									// the "Update" (UARTUpdate).
		}
		else if( uprim->waitTextState == UART_RECEIVE_STRING_RECEIVED_OK ){  //Asks for the received code flag. Must be
			return UART_received;									// called after the "Update" (UARTUpdate).
		}
}

// It writes a message through UART PORT
void UARTReport(uart_prim_t * uprim, char *text){
	uartWriteString( uprim->Uart, text );
}
