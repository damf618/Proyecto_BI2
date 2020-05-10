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
	  Init_Sys,                     // Function that implements the task.
      (const char *)"Init_Sys",     // Text name for the task.
      configMINIMAL_STACK_SIZE*2, // Stack size in words, not bytes.
      0,                          // Parameter passed into the task.
      tskIDLE_PRIORITY+3,         // Priority at which the task is created.
      0                           // Pointer to the task created in the system
   );

   if(res==0){
	   //printf("Error en Task 1");
   }
   // Create a task in freeRTOS with dynamic memory
   res=
   xTaskCreate(
		   Update_Sys,                     // Function that implements the task.
         (const char *)"Update_Sys",     // Text name for the task.
         configMINIMAL_STACK_SIZE*2, // Stack size in words, not bytes.
         0,                          // Parameter passed into the task.
         tskIDLE_PRIORITY+1,         // Priority at which the task is created.
         0                           // Pointer to the task created in the system
      );
   if(res==0){
   	   //printf("Error en Task 2");
      }

   res=
      xTaskCreate(
    		Control_Sys,                     // Function that implements the task.
            (const char *)"Control_Sys",     // Text name for the task.
            configMINIMAL_STACK_SIZE*4, // Stack size in words, not bytes.
            0,                          // Parameter passed into the task.
            tskIDLE_PRIORITY+2,         // Priority at which the task is created.
            0                           // Pointer to the task created in the system
         );
      if(res==0){
      	   //printf("Error en Task 3");
         }

/*
      res=
         xTaskCreate(
         	  Test_Mode,                     // Function that implements the task.
               (const char *)"Test_Mode",     // Text name for the task.
               configMINIMAL_STACK_SIZE*2, // Stack size in words, not bytes.
               0,                          // Parameter passed into the task.
               tskIDLE_PRIORITY+1,         // Priority at which the task is created.
               0                           // Pointer to the task created in the system
            );
         if(res==0){
         	   //printf("Error en Task 4");
            }

      res=
        xTaskCreate(
          CurrentTmode,                     // Function that implements the task.
		  (const char *)"CurrentTmode",     // Text name for the task.
		  configMINIMAL_STACK_SIZE*1, // Stack size in words, not bytes.
		  0,                          // Parameter passed into the task.
		  tskIDLE_PRIORITY+1,         // Priority at which the task is created.
		  0                           // Pointer to the task created in the system
    		  );
      if(res==0){
    	  //printf("Error en Task 5");
      }

      res=
         xTaskCreate(
        	   State_Test,                     // Function that implements the task.
               (const char *)"State_Test",     // Text name for the task.
               configMINIMAL_STACK_SIZE*1, // Stack size in words, not bytes.
               0,                          // Parameter passed into the task.
               tskIDLE_PRIORITY+1,         // Priority at which the task is created.
               0                           // Pointer to the task created in the system
            );
         if(res==0){
         	   //printf("Error en Task 6");
            }
*/


      vTaskStartScheduler(); // Initialize scheduler

   while( true ); // If reach heare it means that the scheduler could not start

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
