/*
 * prs.h
 *
 *  Created on: May 13, 2019
 *      Author: kgraham
 */
#ifndef SRC_HEADER_FILES_PRS_H_
#define SRC_HEADER_FILES_PRS_H_
//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_prs.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct {
	uint8_t			chan_number;	// Channel number of PRS being used
	bool			async;			// Asserted if Asynchronous
	uint32_t		source;			// PRS producer
	uint8_t			signal;			// PRS consumer
	uint8_t			route_loc;		// route of PRS channel to external pin
	bool			route_enable;	// enable out 1 route
} APP_PRS_CHAN_TypeDef ;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void prs_pwm_open(APP_PRS_CHAN_TypeDef	*prs_setup);
#endif
