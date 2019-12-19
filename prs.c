/**
* @file prs.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief setting up Peripheral Reflex System. Setting up Peripheral Reflex System
*
* @details
* Assigning the register/pin values of Peripheral Reflex System
* checking interrupts, more detail about PRS can be found on Reference manual
*
* @note
* This function assign the pins to PRS values which are set in app.c ,
* setting up the channel, routine, route
*
*
* @param prs_setup
* this APP_PRS_TypeDef function is use to setup the PRS getting the values
* previously reference in app.c
*
* @return
* this is a void function and has no return
*
*/

//***********************************************************************************
// Include files
//***********************************************************************************

//**  Standard Libraries

//**  Silicon Labs include files
#include "em_cmu.h"
#include "em_assert.h"

//**  User/develop include files
#include "prs.h"
/******************************************************************************//***
*@brief

*
******************************************************************************/

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************



//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *		initializes and configures the PRS peripheral
 *
 * @details
 *		Configure the PRS peripheral according to the values
 *		in the parameter structure
 *
 * @param prs_setup
 * 		 structure containing  the PRS peripheral
 ******************************************************************************/
void prs_pwm_open(APP_PRS_CHAN_TypeDef	*prs_setup){
		 CMU_ClockEnable (cmuClock_PRS,true);
		 PRS->CH[prs_setup->chan_number].CTRL=0;
		 PRS->CH[prs_setup->chan_number].CTRL=PRS_CH_CTRL_EDSEL_POSEDGE;
		 PRS->CH[prs_setup->chan_number].CTRL |=(prs_setup->async)*PRS_CH_CTRL_ASYNC;
		 PRS->CH[prs_setup->chan_number].CTRL |=prs_setup->source;
		 PRS->CH[prs_setup->chan_number].CTRL |=prs_setup->signal;
		 PRS->SWLEVEL |=prs_setup->chan_number;
		 PRS->ROUTEPEN &= ~0x1 <<(prs_setup->chan_number);
		 PRS->ROUTEPEN |=prs_setup->route_enable<<(prs_setup->chan_number);
		 PRS->ROUTELOC0 |=prs_setup->route_loc;

		 PRS->CTRL &= ~0b1;
		 if((prs_setup->chan_number)<=3) {
			 PRS->ROUTELOC0 &=~0x7F;
			 PRS->ROUTELOC0 |=prs_setup->route_loc<<(8*(prs_setup->chan_number));
		 }
		 else if((prs_setup->chan_number)<=7) {
					 PRS->ROUTELOC1 &=~0x7F<<(8*((prs_setup->chan_number)-4));
					 PRS->ROUTELOC1 |=prs_setup->route_loc<<(8*((prs_setup->chan_number)-4));
				 }
		 else if((prs_setup->chan_number)<=11) {
							 PRS->ROUTELOC2 &=~0x7F<<(8*((prs_setup->chan_number)-8));
							 PRS->ROUTELOC2 |=prs_setup->route_loc<<(8*((prs_setup->chan_number)-8));
						 }
		 else {
			 return;
		 }
		 unsigned int original_swlevel=PRS->SWLEVEL;
		 if((PRS->SWLEVEL &0x01)==0){
			 PRS->SWLEVEL= 0x01;
			 EFM_ASSERT(PRS->SWLEVEL &0x01);
			 PRS->SWLEVEL=original_swlevel;
		 }
		 else{
			 PRS->SWLEVEL=0x0;
			 EFM_ASSERT(!(PRS->SWLEVEL &0x01));
			 PRS->SWLEVEL = original_swlevel;
		 }

}
