
/**
* @file adc.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief Assigning ADC/ resetting and building interrupt
* @details
*  	passed instruction determines the flow of ADC  performing the called function
*/

//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c.h"
#include "Si7021.h"
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "scheduler.h"
#include "gpio.h"
#include "app.h"
#include "adc.h"
#include "ble.h"
#include "adc.h"


uint32_t result = 0;
window_stat window_status = stat_window_out;
uint32_t window_high = 0;
uint32_t window_low = 0;

//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Configures an ADC peripheral for a window switch
 *
 * @details
 * Configures the ADC peripherals and follow a CPU inturrupt
 *
 * @param ADC
 * 	ADC peripheral
 *
 * @param ADC_open
 * 	General ADC open peripheral settings selections
 *
 ******************************************************************************/
void ADC_single_open(ADC_TypeDef *ADC,ADC_OPEN_STRUCT ADC_open){
	if(ADC == ADC0){
		CMU_ClockEnable (cmuClock_ADC0 , true);
	}
    ADC->IEN			  =		0;
    ADC->IFC			  =		ADC_IFC_PROGERR;
    ADC->IFS			  =		ADC_IFS_PROGERR;
    EFM_ASSERT(ADC->IF &ADC_IF_PROGERR);
    ADC->IFC			  =		ADC_IFC_PROGERR;
    ADC->IEN			  =		ADC_IEN_PROGERR;


    ADC_InitSingle_TypeDef	ADC_SIG	;
    ADC_SIG.acqTime		   = 	    ADC_open.acqTime;
    ADC_SIG.diff 		   =	    ADC_open.diff;
	ADC_SIG.fifoOverwrite  = 	    ADC_open.fifoOverwrite;
	ADC_SIG.leftAdjust     =		ADC_open.leftAdjust;
	ADC_SIG.negSel         =		ADC_open.negSel;
	ADC_SIG.posSel         =	    ADC_open.posSel;
	ADC_SIG.prsEnable      =     	ADC_open.prsEnable;
	ADC_SIG.prsSel         =	    ADC_open.prsSel;
	ADC_SIG.reference      =		ADC_open.reference;
	ADC_SIG.rep            =    	ADC_open.rep;
	ADC_SIG.resolution     =    	ADC_open.resolution;
	ADC_SIG.singleDmaEm2Wu =	    ADC_open.singleDmaEm2Wu;
	ADC_InitSingle(ADC, &ADC_SIG);
	EFM_ASSERT(!(ADC->IF & ADC_IF_PROGERR));

	ADC->SINGLECTRLX	   &=		 ~(_ADC_SINGLECTRLX_DVL_MASK);
	ADC->BIASPROG 		   |=		 (_ADC_BIASPROG_GPBIASACC_LOWACC | _ADC_BIASPROG_ADCBIASPROG_SCALE16);
	window_high			    =		 ADC_open.high;
	window_low 			    =		 ADC_open.low;

	ADC->CMPTHR 			= 		 ((window_high << _ADC_CMPTHR_ADGT_SHIFT)  | window_low);
	ADC->SINGLECTRL 	   |=		  ADC_SINGLECTRL_CMPEN;
	ADC_Init_TypeDef adc_t;

	adc_t.em2ClockConfig   = 		ADC_open.em2ClockConfig;
	adc_t.ovsRateSel 	   = 		ADC_open.ovsRateSel;
	adc_t.prescale 		   = 		ADC_open.prescale;
	adc_t.tailgate	       = 		ADC_open.tailgate;
	adc_t.timebase         = 		ADC_open.timebase;
	adc_t.warmUpMode       = 		ADC_open.warmUpMode;

	ADC_Init(ADC, &adc_t);
	//ADC->CTRL 			   |=		 ADC_CTRL_ADCCLKMODE;

	ADC->IEN			   |=		 ADC_open.set;
	EFM_ASSERT(!(ADC->IF & ADC_IF_PROGERR));
    NVIC_EnableIRQ(ADC0_IRQn);
}
/***************************************************************************//**
 * @brief
 *	returns the result
 *
 * @details
 *	return result
 *
 * @param ADC
 * 	ADC peripheral
 *
 * @return
 *  return the result
 ******************************************************************************/
uint32_t ADC_result(ADC_TypeDef *ADC){
	return result;
}

/***************************************************************************//**
 * @brief
 *	starts any ADC conversions
 *
 * @details
 *	starts any ADC conversions
 *
 * @param ADC
 * 	ADC peripheral
 *
 ******************************************************************************/
void ADC_DATA_START(ADC_TypeDef *ADC) {
	ADC_Start(ADC, adcStartSingle);
}
/***************************************************************************//**
 * @brief
 *	Stop any ADC conversions
 *
 * @details
 *	Stop any ADC conversions
 *
 * @param ADC
 * 	ADC peripheral
 *
 ******************************************************************************/
void ADC_DATA_STOP(ADC_TypeDef *ADC) {
	ADC->CMD 				=		   ADC_CMD_SINGLESTOP;
}
/***************************************************************************//**
 * @brief
 *	get windows status
 *
 * @details
 *	Swaps the ADGT and ADLT values and the private variable state and
 *	Returns the result of an ADC peripheral
 *
 * @param ADC
 * 	ADC peripheral desired
 *
 ******************************************************************************/
void WINDOWS_STAT(ADC_TypeDef *ADC) {
	if( window_status == stat_window_in) {
		window_status = stat_window_out;
		ADC->CMPTHR = 0;
		ADC->CMPTHR = ((window_high << _ADC_CMPTHR_ADGT_SHIFT)  | window_low);
	}
	else if(window_status == stat_window_out) {
		window_status = stat_window_in;
		ADC->CMPTHR = 0;
		ADC->CMPTHR = ((window_low << _ADC_CMPTHR_ADGT_SHIFT)  | window_high);
	}
}

/***************************************************************************//**
  *	@brief
  *	ADC0 Interrupt
  *  @details
  *  single conversion for ADC and then adding temt600 event
  *   we are enabling 3 interrupts SINGLE,SINGLECMP,PROGERR which has has events set on app.h
  ******************************************************************************/
void ADC0_IRQHandler(void){
	__disable_irq();
	uint32_t int_flag = ADC0->IF;
	ADC0->IFC = int_flag;
	 if(int_flag & ADC_IF_SINGLE) {
		result = (uint32_t)ADC0->SINGLEDATA;
		add_scheduled_event(TEMT6000_SINGLE_EVT);
		ADC0->SINGLEFIFOCLEAR = ADC_SINGLEFIFOCLEAR_SINGLEFIFOCLEAR;
	 }

	 if(int_flag & ADC_IF_SINGLECMP) {
		add_scheduled_event(TEMT6000_SINGLECMP_EVT);
		if(window_status == stat_window_in) {
			add_scheduled_event(Window_High_EVT);

		}
		else if(window_status == stat_window_out) {
			add_scheduled_event(Window_Low_EVT);
		}
	}
	if(int_flag & ADC_IF_PROGERR) {
		EFM_ASSERT(false);
	}
	__enable_irq();
}
