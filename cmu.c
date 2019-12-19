/**
* @file cmu.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief assigning clock peripherals
* @details
 * assigning clock peripherals
*/
//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// prototypes
//***********************************************************************************

void cmu_open(void){

		//uint32_t	temp_freq;

		// Enabling the High Frequency Peripheral Clock Tree
		CMU_ClockEnable(cmuClock_HFPER, true);

		// By default, LFRCO is enabled, disable the LFRCO oscillator
		CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO

		// Route LF clock to the LF clock tree

		// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

		CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);		// route ULFRCO to proper Low Freq clock tree
//


		//LEUART0 CLOCK
		//CMU_ClockEnable(cmuClock_LEUART0,true);

		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
		//CMU_ClockEnable(cmuClock_LFB, true);
		// Enable the Low Freq clock tree
		//	CMU_ClockEnable(cmuClock_LFA, true);
		//	CMU_ClockEnable(cmuClock_CORELE, true);
		   // CMU_ClockEnable (CMU_HFPERCLKEN0_I2C0 , true);


			// Enable ADC clock
			CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
			CMU_AUXHFRCOFreqSet(cmuAUXHFRCOFreq_1M0Hz);
			//CMU->ADCCTRL |= CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO | _ADC_CTRL_ADCCLKMODE_ASYNC;;
			 CMU->ADCCTRL |= CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO;

			// Enable the Low Freq clock tree
			CMU_ClockEnable(cmuClock_CORELE, true);
}

