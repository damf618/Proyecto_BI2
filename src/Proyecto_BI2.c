/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "Proyecto_BI2.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "sapi.h"
#include "userTasks.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
   boardInit();

   // Create a task in freeRTOS with dynamic memory
   BaseType_t res=
   xTaskCreate(
      myTask,                     // Function that implements the task.
      (const char *)"myTask",     // Text name for the task.
      configMINIMAL_STACK_SIZE*2, // Stack size in words, not bytes.
      0,                          // Parameter passed into the task.
      tskIDLE_PRIORITY+1,         // Priority at which the task is created.
      0                           // Pointer to the task created in the system
   );

   if(res==0){
	   printf("Error en Task 1");
   }
   // Create a task in freeRTOS with dynamic memory
   res=
   xTaskCreate(
         myTask2,                     // Function that implements the task.
         (const char *)"myTask2",     // Text name for the task.
         configMINIMAL_STACK_SIZE*2, // Stack size in words, not bytes.
         0,                          // Parameter passed into the task.
         tskIDLE_PRIORITY+1,         // Priority at which the task is created.
         0                           // Pointer to the task created in the system
      );
   if(res==0){
   	   printf("Error en Task 2");
      }
   vTaskStartScheduler(); // Initialize scheduler

   while( true ); // If reach heare it means that the scheduler could not start

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
