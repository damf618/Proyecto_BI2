/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef ANTIREBOTE_RTOS_H_
#define ANTIREBOTE_RTOS_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "stdint.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define DEBOUNCE_TIME 40

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/
// Enumeration of the different states of the DEBOUNCE MEF
typedef enum{
   STATE_BUTTON_UP,
   STATE_BUTTON_FALLING,
   STATE_BUTTON_DOWN,
   STATE_BUTTON_RISING
} fsmState_t;

// Structure with all the associated variables for the Monitor System Logic
typedef struct{
	bool_t released_flag;
	delay_t delay;
	gpioMap_t button;
	fsmState_t state;
	TickType_t down_time;
	TickType_t up_time;
	TickType_t pressed_time;
	uint32_t contRising;
	uint32_t contFalling;
} debounce_t;

/*=====[Prototypes (declarations) of public functions]=======================*/
/** In case the MEFs reaches an error point it will go back to the initial state.

	@param debounce_t element of type *debounce_t* with all the debounce data types needed
	@note It must be called after fsmButtonInit.
	@see fsmButtonInit.

**/
static void fsmButtonError( debounce_t * pbutton );

/** It sets initial conditions for the entire debounce program, it establish the default values
 *  of timeout. It requires the periodic call of fsmButtonUpdate to work properly

	@param debounce_t element of type *debounce_t* with all the debounce data types needed
	@see fsmButtonUpdate.

**/
void fsmButtonInit( debounce_t * pbutton, gpioMap_t button );

/** It updates the current state of the MEF, evaluates the transition conditions and commands
 * the corresponding outputs.

	@param debounce_t element of type *debounce_t* with all the debounce data types needed
	@note It must be called periodically and must be called the first time after fsmButtonInit.
	@see fsmButtonInit.

**/
void fsmButtonUpdate( debounce_t * pbutton );

/** Function to generate the corresponding actions according to a button pressed state.

	@param debounce_t element of type *debounce_t* with all the debounce data types needed

**/
static void buttonPressed( debounce_t * pbutton );

/** Function to generate the corresponding actions according to a button released state.

	@param debounce_t element of type *debounce_t* with all the debounce data types needed

**/
static void buttonReleased( debounce_t * pbutton );

/** It allows to check if the button was pressed and released. Once the flag was read, it
 * is cleared to register a new pressing and releasing event.

	@param debounce_t element of type *debounce_t* with all the debounce data types needed
	@note It returns 1 if the button was pressed and released, else it returns 0.

**/
bool_t get_flag(debounce_t * pbutton);

/** It forces the pressed and released pushbutton flag on.

	@param debounce_t element of type *debounce_t* with all the debounce data types needed
	@note To read the flag check the get_flag function.
	@see get_flag.

**/
void set_flag(debounce_t * pbutton);


/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* ANTIREBOTE_RTOS_H_ */
