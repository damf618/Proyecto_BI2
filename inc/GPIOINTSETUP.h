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

/*=====[Prototypes (declarations) of public functions]=======================*/

/** In case we are using the TEC4 pushbutton as test mode switch, this function configures
 * the system to generate interrupts associated to this pin.

	@note It set the interrupt priority to 5, check for compatibility with FreeRTOSCONFIG.h.
	@see configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

**/
void GPIOTestInt_INIT(void);

/** Configuration of all the pushbutton needed for the primario4 logic to work properly with
 * RTOS requirements, it sets the maximun interrupt priority to the alarm event, then is the
 * fail event and in the end the normal event.

	@note It assumes you are using the TEC1, TEC2 and TEC3 pushbuttons.

**/
void GPIOInt_INIT(void);

/** The principal objective of this function is to implement a "debounce" strategy through the
 * disable of the interrupts for a period of time (DEBOUNCE TIME) to be enabled after to allow
 * the system to keep working properly.

	@param gpioint it defines if we are going to disable the test event or the entire primario4 logic
	@note It does not enable the interrupts automatically.
	@see GPIOIntEnable

**/
void GPIOIntDisable(char gpioint);

/** The principal objective of this function is to implement a "debounce" strategy through the
 * disable of the interrupts for a period of time (DEBOUNCE TIME) to be enabled after to allow
 * the system to keep working properly.

	@param gpioint it defines if we are going to enable the test event or the entire primario4 logic
	@see GPIOIntDisable

**/
void GPIOIntEnable(char gpioint);

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* GPIO_SETUP_H_ */
