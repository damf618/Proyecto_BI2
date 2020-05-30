/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/12
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "GPIOINTSETUP.h"
#include "chip.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

// ---> Comienzo de funciones LPCOpen para configurar la interrupcion
void GPIOInt_INIT(){
	/*GPIO TEC4 INTERRUPT SETUP*/
	// Configure interrupt channel for the GPIO pin
	Chip_SCU_GPIOIntPinSel( PININT_INDEX, GPIO0_GPIO_PORT, GPIO0_GPIO_PIN);

	// Configure channel interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( PININT_NVIC_NAME);
	NVIC_EnableIRQ( PININT_NVIC_NAME);

	/*GPIO0 GROUP INTERRUPT SETUP TEC1, TEC2, TEC3*/
	// Configure interrupt channel for the GPIO pin in SysCon block
	Chip_GPIO_Init(LPC_GPIO_PORT );

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , GPIO0_GPIO_PORT1, GPIO0_GPIO_PIN1);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , GPIO0_GPIO_PORT2, GPIO0_GPIO_PIN2);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT , GPIO0_GPIO_PORT3, GPIO0_GPIO_PIN3);

	Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUP , GPIO_PORTS, GPIO0_GPIO_PORT1, GPIO0_GPIO_PIN1);
	Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUP , GPIO_PORTS, GPIO0_GPIO_PORT2, GPIO0_GPIO_PIN2);
	Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUP , GPIO_PORTS, GPIO0_GPIO_PORT3, GPIO0_GPIO_PIN3);

	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP , GPIO_PORTS, GPIO0_GPIO_PORT1, 1 <<   GPIO0_GPIO_PIN1);
	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP , GPIO_PORTS, GPIO0_GPIO_PORT2, 1 <<   GPIO0_GPIO_PIN2);
	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP , GPIO_PORTS, GPIO0_GPIO_PORT3, 1 <<   GPIO0_GPIO_PIN3);

	Chip_GPIOGP_SelectAndMode(LPC_GPIOGROUP , AndMode);

	Chip_GPIOGP_SelectEdgeMode(LPC_GPIOGROUP , EdgeMode);

	NVIC_ClearPendingIRQ(GINT0_IRQn );
	NVIC_ClearPendingIRQ(GINT0_IRQn);
	NVIC_EnableIRQ(GINT0_IRQn );

	NVIC_SetPriority(PININT_NVIC_NAME, PRIORIDADES);
	NVIC_SetPriority(GINT0_IRQn, PRIORIDADES);

}
