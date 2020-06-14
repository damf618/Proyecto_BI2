/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef GPIO_H_
#define GPIO_H_

/*=====[Inclusions of public function dependencies]==========================*/

#include "sapi.h"


/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define HIGH_G 1
#define LOW_G 0

#define YELLOW_LIGHT LED1			// GPIO for the Yellow LED
#define RED_LIGHT LED2				// GPIO for the Red LED
#define GREEN_LIGHT LED3			// GPIO for the Green LED

/*=====[Prototypes (declarations) of public functions]=======================*/
/** Function to read the current state of a digital configured pin.

	@param gpio pin we wish to be read.
	@note Based on the sapi library.

**/
bool_t GPIORead( gpioMap_t gpio );

/**  Function to set the output of a digital configured pin.

	@param gpio pin we wish to be set.
	@note Based on the sapi library.

**/
void GPIOWrite( gpioMap_t gpio, bool_t state);

/**  Function to toggle the output of a digital configured pin.

	@param gpio pin we wish to be modified.
	@note Based on the sapi library.

**/
void GPIOToggle(gpioMap_t gpio);

/**  Function to configure the selected pin as digital output.

	@param gpio pin to be configured as output.
	@note Based on the sapi library.

**/
void GPIOOutConfig(gpioMap_t pin);

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_H_ */
