/*=============================================================================
 * Copyright (c) 2020, DANIEL MARQUEZ <DAMF618@GMAIL.COM>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2020/05/09
 * Version: 1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef FSMUARTCODES_H_
#define FSMUARTCODES_H_

/*=====[Inclusions of public function dependencies]==========================*/
#include <sapi.h>
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Prototypes (declarations) of public functions]=======================*/

/** MEF to control the processing of the ASCII Characters that arrives trough the UART
 * Console, it is based on the sapi library and has some modifications towork properly
 *  with FreeRTOS. It has an internal logic to process the expected code (alarm_code:
 *  "al987", fail_code: "fa123", normal_code: "no456") with some tolerance to noise and
 *  "extra" characters.

	@param uprim element of type *uart_prim_t* with the uart_primary data types needed
	@note It requires an external signal (Semaphore) to set the timeout event.
	@see Time_Update

**/
waitForReceiveStringOrTimeoutState_t waitForReceiveStringOrTimeout2( uartMap_t uart, waitForReceiveStringOrTimeout_t* instance, char receiveByte, uart_prim_t * uprim );


/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /* FSMUARTCODES_H_ */
