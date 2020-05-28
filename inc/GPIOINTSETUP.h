/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef GPIO_SETUP_H_
#define GPIO_SETUP_H_

/*=====[Inclusions of public function dependencies]==========================*/


/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

// SCU GROUP
#define GPIO_PORTS 0

#define GPIO0_GPIO_PORT1  0
#define GPIO0_GPIO_PIN1   9

#define GPIO0_GPIO_PORT2  0
#define GPIO0_GPIO_PIN2   4

#define GPIO0_GPIO_PORT3  0
#define GPIO0_GPIO_PIN3   8

#define AndMode 0
#define EdgeMode 0


// SCU
#define GPIO0_SCU_PORT   1
#define GPIO0_SCU_PIN    6
#define GPIO0_SCU_FUNC   SCU_MODE_FUNC0

// GPIO
#define GPIO0_GPIO_PORT  1
#define GPIO0_GPIO_PIN   9

// Interrupt
#define PININT_INDEX         1                  // PININT index used for GPIO mapping
#define PININT_IRQ_HANDLER   GPIO1_IRQHandler   // GPIO interrupt IRQ function name
#define PININT_NVIC_NAME     PIN_INT1_IRQn      // GPIO interrupt NVIC interrupt name

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

void GPIOInt_INIT();

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_SETUP_H_ */
