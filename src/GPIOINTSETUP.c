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

// ---> GPIO Interrupt Configuration
void GPIOInt_INIT(){

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
	NVIC_SetPriority(TEC1_INT, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
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
	NVIC_SetPriority(TEC2_INT, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+2);
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
	NVIC_SetPriority(TEC3_INT, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
	NVIC_EnableIRQ( TEC3_INT);
}

// ---> Test GPIO Interrupt Configuration
void GPIOTestInt_INIT(){
	/*GPIO TEC4 INTERRUPT SETUP*/
	// Configure interrupt channel for the GPIO pin
	Chip_SCU_GPIOIntPinSel( PININT_INDEX, GPIO0_GPIO_PORT, GPIO0_GPIO_PIN);

	// Configure channel interrupt as edge sensitive and falling edge interrupt
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));

	// Enable interrupt in the NVIC
	NVIC_ClearPendingIRQ( TEST_INT);
	NVIC_SetPriority(TEST_INT, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_EnableIRQ( TEST_INT);
}

// ---> Disable Interrupts of the full GPIO group or the test pushbutton.
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

// ---> Enable Interrupts of the full GPIO group or the test pushbutton.
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
