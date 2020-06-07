/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/04/12
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "primario4.h"
#include "antirebote_rtos.h"
#include "gpio.h"
#include "GPIOINTSETUP.h"
#include "Primario_UART.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*=====[Callbacks Variables]==============================*/

volatile static callBackFuncPtr_t TEC1CallbackGPIO = 0;
volatile void* TEC1CallbackGPIOParams = NULL;

volatile static callBackFuncPtr_t TEC2CallbackGPIO = 0;
volatile void* TEC2CallbackGPIOParams = NULL;

volatile static callBackFuncPtr_t TEC3CallbackGPIO = 0;
volatile void* TEC3CallbackGPIOParams = NULL;

/*=============[RTOS_TASKS]==============================*/

// One Shot Special Event ALARM to FAIL Exception
static void MaskedFail(void* taskParmPtr){
	//Receive the Parameter
	dprimario_t * aux = (dprimario_t *)taskParmPtr;
	//------Oneshot
	if(Failcheck(aux))									//If we are no longe in Alarm State but there is a Fail
		xSemaphoreGive( aux->prim_handler.GPIO_INTF);	// present, we change directly to the Fail State

	vTaskDelete(NULL);
}

//RTOS UART Interrupt Associated Task
static void IntTaskUART(void* taskParmPtr )
{
	// ----- Task setup -----------------------------------
	//Receive the Parameter
	dprimario_t * aux = (dprimario_t *)taskParmPtr;

	char datareceived=0;
	// ----- Task repeat for ever -------------------------
	while(1)
	{
		// Has an interrupt ocurred?
		xQueueReceive( aux->prim_handler.UARTQUEUE,&datareceived ,portMAX_DELAY );
		Time_Update(&aux->uart1, datareceived);										//Process the UART Character
	}
}

//RTOS GPIO Interrupt Associated Task
static void IntGPIOPrim(void* taskParmPtr )
{
	//Receive the Parameter
	dprimario_t * aux = (dprimario_t *)taskParmPtr;

	char status=ALL_GOOD;
	char data[2]={0,0};

	while(TRUE) {

		xQueueReceive( aux->prim_handler.GPIOQUEUE,&data ,portMAX_DELAY );

		if(data[1]==GPIO_FALLING){
			switch(data[0]){
			case TEC1_INTF:
				set_flag(&aux->boton1);
				break;
			case TEC2_INTF:
				set_flag(&aux->boton2);
				break;
			case TEC3_INTF:
				set_flag(&aux->boton3);
				break;
			default:
				get_flag(&aux->boton1);
				get_flag(&aux->boton2);
				get_flag(&aux->boton3);
				break;
			}
		}
		else if (data[1]==GPIO_RISING){
			GPIOIntEnable(GROUP);
			//CASO EXCEPCIONAL PARA QUE AL REGRESAR DE UNA ALARMA VERIFICAR SI HAY FALLAS PRESENTES
			if((data[0]==TEC1_INTF)&&(aux->state==ALARM)){
				BaseType_t res= xTaskCreate(
						MaskedFail,						// Function that implements the task.
						(const char *)"MaskedFail",		// Text name for the task.
						configMINIMAL_STACK_SIZE, 		// Stack size in words, not bytes.
						taskParmPtr,		   				// Parameter passed into the task.
						tskIDLE_PRIORITY+5,    			// Priority at which the task is created.
						0 );				 			// Pointer to the task created in the system

				if(res==0){
					status=TASK_ERROR;
				}
				RTOS_Check(aux,status);
			}

		}
		//Antirebote
		NVIC_DisableIRQ(GROUP);
		vTaskDelay( DEBOUNCE_TIME / portTICK_RATE_MS);
		NVIC_EnableIRQ(GROUP);
	}
}

/*=====[Interrupt Handlers and Callbacks]=================*/

//GPIO Interrupt and callback association
static void GPIOInterrupt(char gpio){

	if((gpio == TEC1_INTF )&&(TEC1CallbackGPIO != 0)){
	  	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX1));
	  	(*TEC1CallbackGPIO)(0);		//Pointer to process the TEC1 Interrupt
	}
	if((gpio == TEC2_INTF )&&(TEC2CallbackGPIO != 0)){
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX2));
		(*TEC2CallbackGPIO)(0);		//Pointer to process the TEC2 Interrupt
	}
	if((gpio == TEC3_INTF )&&(TEC3CallbackGPIO != 0)){
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));
		(*TEC3CallbackGPIO)(0);		//Pointer to process the TEC3 Interrupt
	}
}

//Interrupt processing for pushbutton TEC1 (ALARM)
static void GPIOIntProcess1(void* TEC1CallbackGPIOParam ){

	dprimario_t * auxp = (dprimario_t *)TEC1CallbackGPIOParams;

	char aux=TEC1_INTF;
		char edge;
		//Limpiar bandera de Interrupcion
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX1));
		BaseType_t gpioupdate = pdFALSE;
		char debugtry=auxp->prim_handler.value;
		if(xSemaphoreTakeFromISR( auxp->prim_handler.GPIO_INTA, &gpioupdate ))
		{
			edge=GPIO_RISING;
			//Ignorar pulsos durante estado DOWN del boton
			Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX2));
			Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));
		}
		else
		{
			edge=GPIO_FALLING;
			NVIC_DisableIRQ( TEC2_INT);
			NVIC_DisableIRQ( TEC3_INT);
			xSemaphoreGiveFromISR( auxp->prim_handler.GPIO_INTA, &gpioupdate );
		}
		char data[2]={aux,edge};
		//Verificacion para ceder el CPU a la tarea de interrupcion
		xQueueSendFromISR( auxp->prim_handler.GPIOQUEUE, &data, &gpioupdate );
		portYIELD_FROM_ISR( gpioupdate );
}

//Interrupt processing for pushbutton TEC2 (FAIL)
static void GPIOIntProcess2(void *TEC2CallbackGPIOParam ){

	dprimario_t * auxp = (dprimario_t *)TEC2CallbackGPIOParams;
	char aux=TEC2_INTF;
		char edge;
		//Limpiar bandera de Interrupcion
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX2));
		BaseType_t gpioupdate2 = pdFALSE;
		if(xSemaphoreTakeFromISR( auxp->prim_handler.GPIO_INTF, &gpioupdate2 ))
		{
			edge=GPIO_RISING;
			//Ignorar pulsos durante estado DOWN del boton
			Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));
		}
		else
		{
			edge=GPIO_FALLING;
			NVIC_DisableIRQ( TEC3_INT);
			xSemaphoreGiveFromISR( auxp->prim_handler.GPIO_INTF, &gpioupdate2 );
		}
		char data[2]={aux,edge};
		//Verificacion para ceder el CPU a la tarea de interrupcion
		xQueueSendFromISR( auxp->prim_handler.GPIOQUEUE, &data, &gpioupdate2 );
		portYIELD_FROM_ISR( gpioupdate2 );
}

//Interrupt processing for pushbutton TEC1 (NORMAL)
static void GPIOIntProcess3(void *TEC3CallbackGPIOParam ){

	dprimario_t * auxp = (dprimario_t *)TEC3CallbackGPIOParams;
	char aux=TEC3_INTF;
	char edge;
	//Limpiar bandera de Interrupcion
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX3));
	BaseType_t gpioupdate3 = pdFALSE;
	if(xSemaphoreTakeFromISR( auxp->prim_handler.GPIO_INTN, &gpioupdate3 ))
	{
		edge=GPIO_RISING;
	}
	else
	{
		edge=GPIO_FALLING;
		xSemaphoreGiveFromISR( auxp->prim_handler.GPIO_INTN, &gpioupdate3 );
	}
	char data[2]={aux,edge};
	//Verificacion para ceder el CPU a la tarea de interrupcion
	xQueueSendFromISR( auxp->prim_handler.GPIOQUEUE, &data, &gpioupdate3 );
	portYIELD_FROM_ISR( gpioupdate3 );

}

//GPIOInterrupt callback association and parameters configuration
static void GPIOCallbackSet( void* callbackParam1)
 {
	//Function related to the processing of the TEC1 INterrupt and it's parameter
	TEC1CallbackGPIO = GPIOIntProcess1;
	TEC1CallbackGPIOParams = callbackParam1;

	//Function related to the processing of the TEC2 INterrupt and it's parameter
	TEC2CallbackGPIO = GPIOIntProcess2;
	TEC2CallbackGPIOParams = callbackParam1;

	//Function related to the processing of the TEC3 INterrupt and it's parameter
	TEC3CallbackGPIO = GPIOIntProcess3;
	TEC3CallbackGPIOParams = callbackParam1;

 }

//Interrupt processing for the arrive of a UART Character
static void onRx(  void *taskParmPtr )
{
	dprimario_t * aux = (dprimario_t *)TEC1CallbackGPIOParams;
	// Lectura de dato y borrado de flag de interrupcion por UART
	char datareceived=0;
	datareceived = uartRxRead( default_UART_PORT );
	BaseType_t uartupdate = pdFALSE;
	//Verificacion para ceder el CPU a la tarea de interrupcion
	xQueueSendFromISR( aux->prim_handler.UARTQUEUE, &datareceived, &uartupdate );
	portYIELD_FROM_ISR( uartupdate );
}

//Interrupt TEC1 Handler
void  LOGIC1_IRQ_HANDLER(void){
	GPIOInterrupt(TEC1_INTF);
}

//Interrupt TEC2 Handler
void  LOGIC2_IRQ_HANDLER(void){
	GPIOInterrupt(TEC2_INTF);
}

//Interrupt TEC3 Handler
void  LOGIC3_IRQ_HANDLER(void){
	GPIOInterrupt(TEC3_INTF);
}

/*=====[Functions, program entry point after power on or reset]==========*/

// RTOS Environment initialization
static char RTOS_Components_Init(dprimario_t * prim){
		char status=ALL_GOOD;
		// ----- Queue to manage the UART Interrupt Task
		prim->prim_handler.UARTQUEUE = xQueueCreate( 1, sizeof( char ) );
		if( prim->prim_handler.UARTQUEUE == NULL )
			status=QUEUE_ERROR;

		// ----- Queue to manage the principal GPIO Interrupt Task
		char data[2]={0,0};
		prim->prim_handler.GPIOQUEUE = xQueueCreate( 1, sizeof( data ) );
		if( prim->prim_handler.GPIOQUEUE == NULL )
			status=QUEUE_ERROR;

		// ----- Semaphore to manage the GPIO Interrupt edge change
		prim->prim_handler.GPIO_INTA = xSemaphoreCreateBinary();
		if( prim->prim_handler.GPIO_INTA == NULL )
			status=SEMAPHORE_ERROR;

		// ----- Semaphore to manage the GPIO Interrupt edge change
		prim->prim_handler.GPIO_INTF = xSemaphoreCreateBinary();
		if( prim->prim_handler.GPIO_INTF == NULL )
			status=SEMAPHORE_ERROR;

		// ----- Semaphore to manage the GPIO Interrupt edge change
		prim->prim_handler.GPIO_INTN = xSemaphoreCreateBinary();
		if( prim->prim_handler.GPIO_INTN == NULL )
			status=SEMAPHORE_ERROR;

		// ----- Mutex for managing the UART writing resource
		prim->prim_handler.gatekeeper=xSemaphoreCreateRecursiveMutex();
		if( prim->prim_handler.gatekeeper == NULL )
			status=MUTEX_ERROR;

		// ----- Semaphore to manage the UART Message Timeout
		prim->uart1.Msg_Timeout = xSemaphoreCreateBinary();
		if( prim->uart1.Msg_Timeout == NULL )
			status=SEMAPHORE_ERROR;

		// ----- Task Creation
		if(status==ALL_GOOD)
			status=RTOS_Task_Init(prim);

		return status;
}

// RTOS Tasks initialization
static char RTOS_Task_Init(dprimario_t * prim){
	char status =ALL_GOOD;
	BaseType_t res= xTaskCreate(
			IntTaskUART,						// Function that implements the task.
			(const char *)"IntTaskUART",		// Text name for the task.
			configMINIMAL_STACK_SIZE*2, 		// Stack size in words, not bytes.
			(void*)prim,		   			// Parameter passed into the task.
			tskIDLE_PRIORITY+2,    				// Priority at which the task is created.
			prim->prim_handler.IntTaskUARTHandle );	// Pointer to the task created in the system

	if(res==0){
		status= TASK_ERROR;
	}

	res= xTaskCreate(
			IntGPIOPrim,						// Function that implements the task.
			(const char *)"IntGPIOPrim",		// Text name for the task.
			configMINIMAL_STACK_SIZE, 			// Stack size in words, not bytes.
			(void*)prim,		 				// Parameter passed into the task.
			tskIDLE_PRIORITY+2,    				// Priority at which the task is created.
			prim->prim_handler.IntTaskGPIO1Handle ); 	// Pointer to the task created in the system

	if(res==0){
		status= TASK_ERROR;
	}

	return status;
}

// To verify if there's a fail waiting once we exit the alarm state
bool_t Failcheck(dprimario_t * prim){
	if(!GPIORead(FAIL_BUTTON)){
		prim->state=FAIL;
		return TRUE;
	}
	return FALSE;
}

// To turn on or off the LEDs according to the current state
void LEDsON(char x,char y,char z){
	if(x==1)					// if x==1, then the Yellow LED is turned on,
		GPIOWrite(YELLOW_LIGHT,HIGH_G);	// is turned off
	else if(x==0)
		GPIOWrite(YELLOW_LIGHT,LOW_G);

	if(y==1)					// if y==1, then the Red LED is turned on,
		GPIOWrite(RED_LIGHT,HIGH_G);		// is turned off
	else if(y==0)
		GPIOWrite(RED_LIGHT,LOW_G);

	if(z==1)					// if z==1, then the Green LED is turned on,
		GPIOWrite(GREEN_LIGHT, HIGH_G);	// is turned off
	else if(z==0)
		GPIOWrite(GREEN_LIGHT,LOW_G);

}

// Whenever a change of state is made, it resets the transition conditions
static void ResetChange(dprimario_t * prim){
	delayInit( &prim->delay,prim->timeout);  //Reset the timeout transition
	UARTReset(&prim->uart1);						 //Reset the UART Listening Process
	prim->count=0;								 //Reset the count of number of cycles
	prim->COMMFLAG=0;								 //Reset the UART flag
}

// To verify if we are stuck in the middle of a transition (PRE-STATE) or in a
// waiting in a principal state (ALARM, NORMAL ,FAIL).
static void PRESTUCK(dprimario_t * prim){
	if(prim->COMMFLAG==1){						//I got here from an UART request?
		if((prim->state==PREALARM)||
			(prim->state==PREFAIL)||
			(prim->state==PRENORMAL)){			//Go to Fail state
			prim->state = FAIL;
			ResetChange(prim);
			prim->COMMFLAG=1;					//Set the UART Flag interaction
		}										//the reset statement clears COMMFLAG
	}
	else{
		if((prim->state==PREALARM)||
			(prim->state==PREFAIL)||
			(prim->state==PRENORMAL)){ 	//Am i stuck in PRESTATE?
			prim->state = FAIL;			//to Fail state.
			ResetChange(prim);
		}
	}
}

// Verify the transition conditions related to pushbuttons
static void ButtonCheck(dprimario_t * prim, char casea, char casef,char casen ){
	if(!delayRead(&prim->delay)){		//Verify if the Timeout transition limit
		if(get_flag(&prim->boton1)){			//Button pressed?
			prim->state=casea;			//The New state is the Alarm related state
			ResetChange(prim);
		}
		else if(get_flag(&prim->boton2)){	//Button pressed?
			prim->state=casef;			//The New state is the Fail related state
			ResetChange(prim);
		}
		else if(get_flag(&prim->boton3)){	//Button pressed?
			prim->state=casen;			//The New state is the Normal related state
			ResetChange(prim);
		}
	}
	else								//Timeout transition limit reached
		PRESTUCK(prim);
}

// Verify the transition conditions related to uart codes
static void UartCheck(dprimario_t * prim,char mode,char nextmode ,char uartstate ){
	if(prim->uart1.mode==mode){							//Am I listening to Alarm UART codes?
		if((UARTRead(&prim->uart1)==UART_received)){  	//Was an Alarm code received?
			prim->state=uartstate;						//The New state is the Alarm related state
			ResetChange(prim);
			prim->COMMFLAG=1;							//Set the UART Flag interaction
		}

		else if(UARTRead(&prim->uart1)==UART_timeout){	//The UART Listening timeout was reached?
			prim->count++;
			UARTChange(&prim->uart1,nextmode);			//Set the UART Listening Process to
		}												//monitor Fail codes
	}
	if (prim->count>=CYCLES)							//If the number specified of cycles were reached
		PRESTUCK(prim);
}

// Verify the 3 Uart Codes and the button state transitions
static void FullCheck(dprimario_t * prim,char casea, char casef,char casen){
	ButtonCheck(prim,casea,casef,casen);
	UartCheck(prim,ALARMS,FAILURES,casea);
	UartCheck(prim,FAILURES,NORMALS,casef);
	UartCheck(prim,NORMALS,ALARMS,casen);
}

//update the MEFSs,
void primUpdates(dprimario_t * pPrimario){

	fsmButtonUpdate(&pPrimario->boton1);			//Update of all the MEFSs involved
	fsmButtonUpdate(&pPrimario->boton2);
	fsmButtonUpdate(&pPrimario->boton3);
}

//RTOS Verification for primary device
bool_t RTOS_Check(dprimario_t * pPrimario,char status_init){

	bool_t RTOS_status=TRUE;
	switch(status_init){
		case SEMAPHORE_ERROR:
			UARTReport( &pPrimario->uart1, "\r\n Error Code: SEMAPHORE_ERROR \r\n" );
			while(1){
				LEDsON(1,1,1);
			}
			break;

		case SEMAPHOREC_ERROR:
			UARTReport( &pPrimario->uart1, "\r\n Error Code: SEMAPHORE_COUNTER_ERROR \r\n" );
			while(1){
				LEDsON(1,1,1);
			}
			break;

		case MUTEX_ERROR:
			UARTReport( &pPrimario->uart1, "\r\n Error Code: MUTEX_ERROR \r\n" );
			while(1){
				LEDsON(1,1,1);
			}
			break;

		case QUEUE_ERROR:
			UARTReport( &pPrimario->uart1, "\r\n Error Code: QUEUE_ERROR \r\n" );
			while(1){
				LEDsON(1,1,1);
			}
			break;
		case TASK_ERROR:
			UARTReport( &pPrimario->uart1, "\r\n Error Code: TASK_ERROR \r\n" );
			while(1){
				LEDsON(1,1,1);
			}
			break;
		case ALL_GOOD:
			UARTReport( &pPrimario->uart1, "\r\n Tasks Handled Correctly \r\n" );
			break;

		default:
			UARTReport( &pPrimario->uart1, "\r\n Error Code: NON_EXPECTED_ERROR \r\n" );
			while(1){
				LEDsON(1,1,1);
			}
			break;

		}

	return RTOS_status;
}

// It sets initial conditions for the entire program
bool_t primInit(dprimario_t * pPrimario)
{
	char status_init=ALL_GOOD;
	if(NULL==pPrimario)
		return FALSE;

	pPrimario->state=INITIAL_DEFAULT_STATE;
	pPrimario->timeout= DEF_TIMEOUT;
	delayInit( &pPrimario->delay,pPrimario->timeout);
	LEDsON(0,0,0);
	fsmButtonInit(&pPrimario->boton1,ALARM_BUTTON);						//Initialize buttons with "antirebote" protocol
	fsmButtonInit(&pPrimario->boton2,FAIL_BUTTON);
	fsmButtonInit(&pPrimario->boton3,NORMAL_BUTTON);
	UARTInit( &pPrimario->uart1,ALARMS,default_UART_PORT, pPrimario->timeout);	//Initialize uart1 through UART_USB

	// Set of a callback for the event of receiving an UART character
	uartCallbackSet(UART_USB, UART_RECEIVE, onRx, pPrimario);
	// Enabling UART_USB Interruptions
	uartInterrupt(UART_USB, true);
	// Enabling GPIO Interruptions
	GPIOInt_INIT();

	status_init=RTOS_Components_Init(pPrimario);
	RTOS_Check(pPrimario,status_init);

	pPrimario->prim_handler.value=54;
	// Set of a callback paramete rfor gpio events
	GPIOCallbackSet( pPrimario);
	return 1;
}

// The MEFS logic, execute the actions related to the state
// and verifies if there is any possible transition.
bool_t primControl(dprimario_t * pPrimario){

	if(NULL==pPrimario)
		return 0;

	switch( pPrimario->state ) {

		case NORMAL:
			LEDsON(0,0,1);
			FullCheck (pPrimario,PREALARM,PREFAIL,NORMAL);
			break;
		case ALARM:
			LEDsON(0,1,0);
			FullCheck (pPrimario,ALARM,PREFAIL,PRENORMAL);
			break;
		case FAIL:
			LEDsON(1,0,0);
			FullCheck (pPrimario,PREALARM,FAIL,PRENORMAL);
			break;
		case PRENORMAL:
			FullCheck (pPrimario,PREALARM,PREFAIL,NORMAL);
			break;
		case PREALARM:
			FullCheck (pPrimario,ALARM,PREFAIL,PRENORMAL);
			break;
		case PREFAIL:
			FullCheck (pPrimario,PREALARM,FAIL,PRENORMAL);
			break;
		default:
			pPrimario->state=FAIL;
		}
	return 1;
}
