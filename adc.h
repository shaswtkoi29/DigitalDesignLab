/*
 * adc.h
 *
 *  Created on: Nov 12, 2019
 *      Author: shaswat
 */

#ifndef SRC_HEADER_FILES_ADC_H_
#define SRC_HEADER_FILES_ADC_H_

// defined files
//***********************************************************************************
#include <stdint.h>
#include "gpio.h"
#include "em_adc.h"

#include "sleep_routines.h"
//#include ADC_FREQ							1000;

//const bool stat_window_in =   0;
//const bool stat_window_out =  1;



//***********************************************************************************
// global variables
//***********************************************************************************

typedef struct {
	ADC_AcqTime_TypeDef			acqTime;
	bool					    diff;
    bool						fifoOverwrite;
	bool						leftAdjust;
	ADC_NegSel_TypeDef			negSel;
	ADC_PosSel_TypeDef 			posSel;
	bool						prsEnable;
	ADC_PRSSEL_TypeDef			prsSel;
	ADC_Ref_TypeDef				reference;
	bool						rep;
	ADC_Res_TypeDef			    resolution;
	bool						singleDmaEm2Wu;
	ADC_OvsRateSel_TypeDef      ovsRateSel;
	ADC_Warmup_TypeDef          warmUpMode;
	uint8_t                     timebase;
	uint8_t                     prescale;
	bool                        tailgate;
	ADC_EM2ClockConfig_TypeDef  em2ClockConfig;
	uint32_t					set;
	uint16_t					high;
	uint16_t					low;
} ADC_OPEN_STRUCT ;

typedef enum {
	stat_window_in = 0,
	stat_window_out = 1
}window_stat;
//***********************************************************************************
// function prototypes
//***********************************************************************************
void ADC_DATA_START(ADC_TypeDef *ADC) ;
void ADC_DATA_STOP(ADC_TypeDef *ADC);
void WINDOWS_STAT(ADC_TypeDef *ADC);
void ADC_single_open(ADC_TypeDef *ADC,ADC_OPEN_STRUCT ADC_open);
uint32_t ADC_result(ADC_TypeDef *ADC);
void ADC0_IRQHandler(void);

#endif /* SRC_HEADER_FILES_ADC_H_ */
