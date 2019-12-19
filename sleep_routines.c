/**
* @file sleep_routines.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief sleep_routine to block mode, unblock mode current sleep state, and entering sleep mode
* @details
* void sleep_open(void) : Initialize the sleep_routines private
* variable, lowest_energy_mode[];
* void sleep_block_mode(unsigned int EM) : Utilized by a
* peripheral to prevent the Pearl Gecko going into that sleep mode
* while the peripheral is active.
* void sleep_unblock_mode(unsigned int EM) : Utilized to release
* the processor from going into a sleep mode with a peripheral is
* no longer active.
* void enter_sleep(void) : Function to enter sleep
* unsigned int current_block_energy_mode(void) : Function that
* returns which energy mode that the current system cannot enter
* @note
* this file goes into different sleep state after entering interrupts
*
* @return
* returns the current EM state
*/
  /************************************************************************** * @file sleep.c *************************************************************************** *
   * @section License *
   * (C) Copyright 2015 Silicon Labs, http://www.silabs.com
   *  *************************************************************************** *
   * Permission is granted to anyone to use this software for any purpose,
   * including commercial applications, and to alter it and redistribute it *
   *  freely, subject to the following restrictions: *
   *  * 1. The origin of this software must not be misrepresented;
   *  you must not * claim that you wrote the original software. *
   *   2. Altered source versions must be plainly marked as such, and must not be *
   *    misrepresented as being the original software. *
   *    3. This notice may not be removed or altered from any source distribution. * *
   *    DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no *
   *     obligation to support this Software. Silicon Labs is providing the
   *     Software "AS IS", with no express or implied warranties of any kind, *
   *    including, but not limited to, any implied warranties of merchantability *
   *    or fitness for any particular purpose or warranties against infringement *
   *    of any proprietary rights of a third party. * *
   *    Silicon Labs will not be liable for any consequential, incidental, or *
   *    special damages, or any other relief, or for any claim by any third party, *
   *    arising from your use of this Software.
   *    * **************************************************************************/
# include "sleep_routines.h"
//# include "sleep_routines.c"
# include "em_emu.h"
# include "em_assert.h"
static unsigned int lowest_enegrgy_mode[MAX_ENERGY_MODES];

/***************************************************************************//**
 * @details
  * Sets all the lowest_energy_mode  to the  state
 ******************************************************************************/
void sleep_open(void){
	lowest_enegrgy_mode[EM0]=0;
	lowest_enegrgy_mode[EM1]=0;
	lowest_enegrgy_mode[EM2]=0;
	lowest_enegrgy_mode[EM3]=0;
	lowest_enegrgy_mode[EM4]=0;
	lowest_enegrgy_mode[MAX_ENERGY_MODES]=0;
}
/***************************************************************************//**
 * @brief
 *	Blocks the sleep mode EM
 *
 * @details
 * 	making sure that EM mode given is between
 * 	 0 and the max   energy modes
 *
 * @param EM
 * 	unsigned int  EM
 *
 ******************************************************************************/
void sleep_block_mode(unsigned int EM){
	EFM_ASSERT(EM<=MAX_ENERGY_MODES);

	lowest_enegrgy_mode[EM] = 	BLOCK;
	EFM_ASSERT(lowest_enegrgy_mode[EM]<=10);
}
/***************************************************************************//**
 * @brief
 *	Unblocks the sleep mode EM
 *
 * @details
 * 	making sure that EM mode given is between
 * 	 0 and the max enegry modes
 *
 * @param EM
 * 	unsigned int EM
 ******************************************************************************/
void sleep_unblock_mode(unsigned int EM){
		EFM_ASSERT(EM <= MAX_ENERGY_MODES);


		lowest_enegrgy_mode[EM] = UNBLOCK;
	EFM_ASSERT(lowest_enegrgy_mode[EM]>=0);
}
/***************************************************************************//**
 * @brief
 *	Enters the Lowest Sleep Mode
 *
 * @details
 *	Enters the lowest energy mode
 *
 ******************************************************************************/
void enter_sleep(void){
	if (lowest_enegrgy_mode[EM0]>0) return;
		else if (lowest_enegrgy_mode[EM1]>0) return;
		else if (lowest_enegrgy_mode[EM2]>0) {
			EMU_EnterEM1();
			return;
		}
		else if (lowest_enegrgy_mode[EM3]>0){
			EMU_EnterEM2(true);
			return;
		}
		else{
			EMU_EnterEM3(true);

		}
	return;
}
/***************************************************************************//**
 * @brief
 *  FInds the current lowest energy mode
 *
 * @details
 *	loops through to find the unblocked mode
 *
 * @return
 * current energy mode
 ******************************************************************************/
unsigned int current_block_energy_mode(void){
	unsigned int i=0;
	for(i=0;i< MAX_ENERGY_MODES;i++){
		if(lowest_enegrgy_mode[i] !=0) return i;
	}
	return(MAX_ENERGY_MODES-1);

	//return 0;
}
