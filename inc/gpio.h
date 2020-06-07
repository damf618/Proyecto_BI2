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

#define TEST_LIGHT LEDB				// GPIO for the Test LED


/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/

bool_t GPIORead( gpioMap_t gpio );
void GPIOWrite( gpioMap_t gpio, bool_t state);
void GPIOToggle(gpioMap_t gpio);

void GPIOOutConfig(gpioMap_t pin);
/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_H_ */
