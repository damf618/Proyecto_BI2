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

typedef enum{
   STATE_BUTTON_UP,
   STATE_BUTTON_FALLING,
   STATE_BUTTON_DOWN,
   STATE_BUTTON_RISING
} fsmState_t;

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

void fsmButtonError( debounce_t * pbutton );
void fsmButtonInit( debounce_t * pbutton, gpioMap_t button );
void fsmButtonUpdate( debounce_t * pbutton );
static void buttonPressed( debounce_t * pbutton );
static void buttonReleased( debounce_t * pbutton );
bool_t get_flag(debounce_t * pbutton);

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* ANTIREBOTE_RTOS_H_ */
