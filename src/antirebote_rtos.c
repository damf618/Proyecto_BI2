/* Copyright 2018, Eric Pernia.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "sapi.h"
#include "FreeRTOS.h"
#include "antirebote_rtos.h"
#include "gpio.h"

/*TODO implement Semaphores/Interrupts to reuse the code */

/* Function to execute once the button was pressed*/
void buttonPressed( debounce_t * pbutton )
{
	pbutton->down_time = xTaskGetTickCount();
}

/* Function to execute once the button was released*/
void buttonReleased( debounce_t * pbutton )
{
	pbutton->up_time = xTaskGetTickCount();
	pbutton->pressed_time = pbutton->up_time - pbutton->down_time;
	pbutton->released_flag=1;
}

/* Function to detect if a pushbutton was pressed*/
bool_t get_flag(debounce_t * pbutton)
{
	if(pbutton->released_flag){
		pbutton->released_flag=0;
		return 1;
	}
	else
		return 0;
}

/* Function to force the signal of pushbutton pressed*/
void set_flag(debounce_t * pbutton)
{
	pbutton->released_flag=1;
}

/* Function to correct any fault in the MEFs transitions */
static void fsmButtonError( debounce_t * pbutton )
{
	pbutton->state = STATE_BUTTON_UP;
}

/* Function to start the full debounce logic*/
void fsmButtonInit( debounce_t * pbutton, gpioMap_t button )
{
	pbutton->button=button;
	pbutton->contFalling = 0;
	pbutton->contRising = 0;
	pbutton->released_flag=0;
	pbutton->up_time=0;
	pbutton->down_time=0;
	pbutton->pressed_time=0;
	pbutton->state = STATE_BUTTON_UP;  // Set initial state
}

// FSM Update Sate Function
void fsmButtonUpdate( debounce_t * pbutton )
{
    switch( pbutton->state )
    {
        case STATE_BUTTON_UP:
            /* CHECK TRANSITION CONDITIONS */
            if( !GPIORead( pbutton->button ) )
            {
            	pbutton->state = STATE_BUTTON_FALLING;
            }
            break;

        case STATE_BUTTON_FALLING:
            /* CHECK TRANSITION CONDITIONS */
            if( pbutton->contFalling >= DEBOUNCE_TIME )
            {
                if( !GPIORead( pbutton->button ) )
                {
                	pbutton->state = STATE_BUTTON_DOWN;

                    /* ACCION DEL EVENTO !*/
                    buttonPressed(pbutton);
                }
                else
                {
                	pbutton->state = STATE_BUTTON_UP;
                }

                pbutton->contFalling = 0;
            }

            pbutton->contFalling++;
            break;

        case STATE_BUTTON_DOWN:
			/* CHECK TRANSITION CONDITIONS */
			if( GPIORead( pbutton->button ) )
			{
				pbutton->state = STATE_BUTTON_RISING;
			}
			break;

        case STATE_BUTTON_RISING:
            /* CHECK TRANSITION CONDITIONS */

            if( pbutton->contRising >= DEBOUNCE_TIME )
            {
                if( GPIORead( pbutton->button ) )
                {
                	pbutton->state = STATE_BUTTON_UP;

                    /* ACCION DEL EVENTO ! */
                    buttonReleased(pbutton);
                }
                else
                {
                	pbutton->state = STATE_BUTTON_DOWN;
                }
                pbutton->contRising = 0;
            }
            pbutton->contRising++;
            break;

        default:
            fsmButtonError(pbutton);
            break;
    }
}
