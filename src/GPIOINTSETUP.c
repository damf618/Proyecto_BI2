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
#include "FreeRTOSConfig.h"
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
	NVIC_ClearPendingIRQ( TEST_INT);
	NVIC_SetPriority(TEST_INT, configMAX_SYSCALL_INTERRUPT_PRIORITY+5);
	NVIC_EnableIRQ( TEST_INT);

/****************TEC1*************************/
	/*GPIO TEC1 INTERRUPT SETUP*/
	// Configure interrupt channel for the GPIO pin
	Chip_SCU_GPIOIntPinSel( PININT_INDEX1, GPIO0_GPIO_PORT1, GPIO0_GPIO_PIN1);


	// Configure channel interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX1));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX1));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX1));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX1));

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( TEC1_INT);
	NVIC_SetPriority(TEC1_INT, configMAX_SYSCALL_INTERRUPT_PRIORITY+5);
	NVIC_EnableIRQ( TEC1_INT);

/****************TEC2************************/
	// Configure interrupt channel for the GPIO pin
	Chip_SCU_GPIOIntPinSel( PININT_INDEX2, GPIO0_GPIO_PORT2, GPIO0_GPIO_PIN2);

	// Configure channel interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX2));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX2));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX2));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX2));

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( TEC2_INT);
	NVIC_SetPriority(TEC2_INT, configMAX_SYSCALL_INTERRUPT_PRIORITY+6);
	NVIC_EnableIRQ( TEC2_INT);

	/****************TEC3************************/
	// Configure interrupt channel for the GPIO pin
	Chip_SCU_GPIOIntPinSel( PININT_INDEX3, GPIO0_GPIO_PORT3, GPIO0_GPIO_PIN3);

	// Configure channel interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( TEC3_INT);
	NVIC_SetPriority(TEC3_INT, configMAX_SYSCALL_INTERRUPT_PRIORITY+7);
	NVIC_EnableIRQ( TEC3_INT);
}

void GPIOIntDisable(char gpioint){
	if(gpioint==GROUP){
		/* Clear pending irq channel interrupts */
		NVIC_ClearPendingIRQ(TEC1_INT);
		NVIC_ClearPendingIRQ(TEC2_INT);
		NVIC_ClearPendingIRQ(TEC3_INT);
		NVIC_DisableIRQ( TEC1_INT);
		NVIC_DisableIRQ( TEC2_INT);
		NVIC_DisableIRQ( TEC3_INT);
	}
	if(gpioint==SINGLE){
		/* Clear pending irq channel interrupts */
		NVIC_ClearPendingIRQ(TEST_INT);
		NVIC_DisableIRQ( TEST_INT);
	}
}

void GPIOIntEnable(char gpioint){
	if(gpioint==GROUP){
		/* Clear pending irq channel interrupts */
		NVIC_ClearPendingIRQ(TEC1_INT);
		NVIC_ClearPendingIRQ(TEC2_INT);
		NVIC_ClearPendingIRQ(TEC3_INT);
		NVIC_EnableIRQ( TEC1_INT);
		NVIC_EnableIRQ( TEC2_INT);
		NVIC_EnableIRQ( TEC3_INT);
	}if(gpioint==SINGLE)
		/* Clear pending irq channel interrupts */
		NVIC_ClearPendingIRQ(TEST_INT);
		NVIC_EnableIRQ( TEST_INT);
}
