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

// GPIO
#define GPIO0_GPIO_PORT  1
#define GPIO0_GPIO_PIN   9

#define GPIO0_GPIO_PORT1  0
#define GPIO0_GPIO_PIN1   4

#define GPIO0_GPIO_PORT2  0
#define GPIO0_GPIO_PIN2   8

#define GPIO0_GPIO_PORT3  0
#define GPIO0_GPIO_PIN3   9

// Interrupt
#define PININT_INDEX			1                  // PININT index used for GPIO mapping
#define TEST_IRQ_HANDLER		GPIO1_IRQHandler   // GPIO interrupt IRQ function name
#define TEST_INT				PIN_INT1_IRQn      // GPIO interrupt NVIC interrupt name

#define PININT_INDEX1			0                  // PININT index used for GPIO mapping
#define LOGIC1_IRQ_HANDLER		GPIO0_IRQHandler   // GPIO interrupt IRQ function name
#define TEC1_INT				PIN_INT0_IRQn

#define PININT_INDEX2			2                  // PININT index used for GPIO mapping
#define LOGIC2_IRQ_HANDLER		GPIO2_IRQHandler   // GPIO interrupt IRQ function name
#define TEC2_INT				PIN_INT2_IRQn

#define PININT_INDEX3			3                  // PININT index used for GPIO mapping
#define LOGIC3_IRQ_HANDLER		GPIO3_IRQHandler   // GPIO interrupt IRQ function name
#define TEC3_INT				PIN_INT3_IRQn

#define GROUP 0
#define SINGLE 1

#define TEC1_INTF 0
#define TEC2_INTF 1
#define TEC3_INTF 2

#define GPIO_FALLING 1
#define GPIO_RISING 0

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

/*=====[Prototypes (declarations) of public functions]=======================*/

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

void GPIOInt_INIT();

void GPIOIntDisable(char gpioint);

void GPIOIntEnable(char gpioint);

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_SETUP_H_ */
