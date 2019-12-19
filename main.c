/***************************************************************************//**
 * @file main.c
 * @author Shaswat Koirala
 * @date December 12th, 2019
 * @brief setting up leuart, I2C, SI7021,PRS,GPIO,CMU,BLE,BLE_Communication,APP,ADC
 * sleep_routines, schedule peripherals for reading
 * the command from BLE terminal, so it displays temperature as per user command
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* System include statements */
#include <stdint.h>
#include <stdbool.h>
#include<stdio.h>

/* Silicon Labs include statements */
#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_letimer.h"

//#include "retargetserial.h"
#include "em_assert.h"
#include "bsp.h"
/* The developer's include statements */
#include "main.h"
#include "app.h"
#include "scheduler.h"
#include "sleep_routines.h"


int main(void)
{
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

  /* Chip errata */
  CHIP_Init();

  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Init DCDC regulator and HFXO with kit specific parameters */
  /* Initialize DCDC. Always start in low-noise mode. */
  EMU_EM23Init_TypeDef em23Init = EMU_EM23INIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);
  em23Init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  EMU_EM23Init(&em23Init);
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFRCO and disable HFRCO */
  CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  CMU_OscillatorEnable(cmuOsc_HFXO, false, false);

  /* Call application program to open / initialize all required peripheral */
  scheduler_open();
  app_peripheral_setup();
  sleep_open();

 EFM_ASSERT(get_scheduled_events() & BOOT_UP_EVT);

  /* Infinite blink loop */
  while (1) {

		 // EMU_EnterEM2(true);

		  if(!get_scheduled_events()){
			  enter_sleep();
		  }
		  if(BOOT_UP_EVT==get_scheduled_events()){
				  //LETIMER0_IRQHandler();
			  scheduled_boot_up_evt();
		  }
		  if(LETIMER0_UF_EVT==get_scheduled_events()){
			  //LETIMER0_IRQHandler();
			  scheduled_letimer0_uf_evt();
		  }
		  if(LETIMER0_COMP0_EVT==get_scheduled_events()){
			  //LETIMER0_IRQHandler();
			  scheduled_letimer0_comp0_evt();
		   }
		  if(LETIMER0_COMP1_EVT==get_scheduled_events()){
			  //LETIMER0_IRQHandler();
			  scheduled_letimer0_comp1_evt();
		   }
		  if(SI7021_TEMP_DONE==get_scheduled_events()){
			  //LETIMER0_IRQHandler();
			  scheduled_si7021_temp_evt();
		  }
		  if(LEUART0_TX_DONE_EVT==get_scheduled_events()){
					  //LETIMER0_IRQHandler();
			      LEUART_TX_DONE_EVT();
		  }
//		  if(LEUART0_SIG == get_scheduled_events() ){
//			       LEUART0_SIG_DONE();
//		  }
//		  if(LEUART0_START == get_scheduled_events() ){
//			  LEUART0_START_DONE();
//		  }
  	   }
}
