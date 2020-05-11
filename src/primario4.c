/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/12
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "primario4.h"
#include "antirebote_rtos.h"
#include "gpio.h"
#include "Primario_UART.h"
#include "sapi.h"

/*=====[Definition macros of private constants]==============================*/


/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Functions, program entry point after power on or reset]==========*/

// To turn on or off the LEDs according to the current state
void LEDsON(char x,char y,char z){
	if(x==1)					// if x==1, then the Yellow LED is turned on,
		GPIOWrite(YELLOW_LIGHT,HIGH_G);	// is turned off
	else if(x==0)
		GPIOWrite(YELLOW_LIGHT,LOW_G);

	if(y==1)					// if y==1, then the Red LED is turned on,
		GPIOWrite(RED_LIGHT,HIGH_G);		// is turned off
	else if(y==0)
		GPIOWrite(RED_LIGHT,LOW_G);

	if(z==1)					// if z==1, then the Green LED is turned on,
		GPIOWrite(GREEN_LIGHT, HIGH_G);	// is turned off
	else if(z==0)
		GPIOWrite(GREEN_LIGHT,LOW_G);

}

// Whenever a change of state is made, it resets the transition conditions
static void ResetChange(dprimario_t * prim){
	delayInit( &prim->delay,prim->timeout);  //Reset the timeout transition
	UARTReset(&prim->uart1);						 //Reset the UART Listening Process
	prim->count=0;								 //Reset the count of number of cycles
	prim->COMMFLAG=0;								 //Reset the UART flag
}

// To verify if we are stuck in the middle of a transition (PRE-STATE) or in a
// waiting in a principal state (ALARM, NORMAL ,FAIL).
static void PRESTUCK(dprimario_t * prim){
	if(prim->COMMFLAG==1){						//I got here from an UART request?
		if((prim->state==PREALARM)||
			(prim->state==PREFAIL)||
			(prim->state==PRENORMAL)){			//Go to Fail state
			prim->state = FAIL;
			ResetChange(prim);
			prim->COMMFLAG=1;					//Set the UART Flag interaction
		}										//the reset statement clears COMMFLAG
	}
	else{
		if((prim->state==PREALARM)||
			(prim->state==PREFAIL)||
			(prim->state==PRENORMAL)){ 	//Am i stuck in PRESTATE?
			prim->state = FAIL;			//to Fail state.
			ResetChange(prim);
		}
	}
}

// Verify the transition conditions related to pushbuttons
static void ButtonCheck(dprimario_t * prim, char casea, char casef,char casen ){
	if(!delayRead(&prim->delay)){		//Verify if the Timeout transition limit
		if(get_flag(&prim->boton1)){			//Button pressed?
			prim->state=casea;			//The New state is the Alarm related state
			ResetChange(prim);
		}
		else if(get_flag(&prim->boton2)){	//Button pressed?
			prim->state=casef;			//The New state is the Fail related state
			ResetChange(prim);
		}
		else if(get_flag(&prim->boton3)){	//Button pressed?
			prim->state=casen;			//The New state is the Normal related state
			ResetChange(prim);
		}
	}
	else								//Timeout transition limit reached
		PRESTUCK(prim);
}

// Verify the transition conditions related to uart codes

static void UartCheck(dprimario_t * prim,char mode,char nextmode ,char uartstate ){
	if(prim->uart1.mode==mode){						//Am I listening to Alarm UART codes?
		if((UARTRead(&prim->uart1)==UART_received)){  //Was an Alarm code received?
			prim->state=uartstate;					//The New state is the Alarm related state
			ResetChange(prim);
			prim->COMMFLAG=1;							//Set the UART Flag interaction
		}

		else if(UARTRead(&prim->uart1)==UART_timeout){//The UART Listening timeout was reached?
			prim->count++;
			UARTChange(&prim->uart1,nextmode);		//Set the UART Listening Process to
		}										//monitor Fail codes
	}

	if (prim->count>=CYCLES)							//If the number specified of cycles were reached
		PRESTUCK(prim);
}

// Verify the 3 Uart Codes and the button state transitions
static void FullCheck(dprimario_t * prim,char casea, char casef,char casen){
	ButtonCheck(prim,casea,casef,casen);
	UartCheck(prim,ALARMS,FAILURES,casea);
	UartCheck(prim,FAILURES,NORMALS,casef);
	UartCheck(prim,NORMALS,ALARMS,casen);
}

//update the MEFSs,
void primUpdates(dprimario_t * pPrimario){

	fsmButtonUpdate(&pPrimario->boton1);			//Update of all the MEFSs involved
	fsmButtonUpdate(&pPrimario->boton2);
	fsmButtonUpdate(&pPrimario->boton3);
//	UARTUpdate(&pPrimario->uart1);
}

// It sets initial conditions for the entire program
bool_t primInit(dprimario_t * pPrimario)
{

	if(NULL==pPrimario)
		return 0;

	pPrimario->state=INITIAL_DEFAULT_STATE;
	pPrimario->timeout= DEF_TIMEOUT;
	delayInit( &pPrimario->delay,pPrimario->timeout);
	LEDsON(0,0,0);
	fsmButtonInit(&pPrimario->boton1,ALARM_BUTTON);	//Initialize buttons with "antirebote" protocol
	fsmButtonInit(&pPrimario->boton2,FAIL_BUTTON);
	fsmButtonInit(&pPrimario->boton3,NORMAL_BUTTON);
	UARTInit( &pPrimario->uart1,ALARMS,UART_USB, pPrimario->timeout); //Initialize uart1 through UART_USB
	UARTReport( &pPrimario->uart1, "\r\nInital process completed successfully \r\n" );
	return 1;
}

// The MEFS logic, execute the actions related to the state
// and verifies if there is any possible transition.
bool_t primControl(dprimario_t * pPrimario){

	if(NULL==pPrimario)
		return 0;

	switch( pPrimario->state ) {

		case NORMAL:
			LEDsON(0,0,1);
			FullCheck (pPrimario,PREALARM,PREFAIL,NORMAL);
			break;
		case ALARM:
			LEDsON(0,1,0);
			FullCheck (pPrimario,ALARM,PREFAIL,PRENORMAL);
			break;
		case FAIL:
			LEDsON(1,0,0);
			FullCheck (pPrimario,PREALARM,FAIL,PRENORMAL);
			break;
		case PRENORMAL:
			FullCheck (pPrimario,PREALARM,PREFAIL,NORMAL);
			break;
		case PREALARM:
			FullCheck (pPrimario,ALARM,PREFAIL,PRENORMAL);
			break;
		case PREFAIL:
			FullCheck (pPrimario,PREALARM,FAIL,PRENORMAL);
			break;
		default:
			pPrimario->state=FAIL;
		}
	return 1;
}
