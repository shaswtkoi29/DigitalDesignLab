//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SRC_HEADER_FILES_LETIMER_H_
#define SRC_HEADER_FILES_LETIMER_H_
#include "em_letimer.h"
#include <stdint.h>

//***********************************************************************************
// defined files
//***********************************************************************************
#define LETIMER_HZ		1000	// Must set the Hz to the clock being used by LETIMER0

//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct {
	bool 			debugRun;			// True = keep LETIMER running will halted
	bool 			enable;				// enable the LETIMER upon completion of open
	uint8_t			out_pin_route0;		// out 0 route to gpio port/pin
	uint8_t			out_pin_route1;		// out 1 route to gpio port/pin
	bool			out_pin_0_en;		// enable out 0 route
	bool			out_pin_1_en;		// enable out 1 route
	float			period;				// seconds
	float			active_period;		// seconds

	bool 			comp0_irq_enable;	//enable interrupt on comp0 interrupt
	uint32_t		comp0_evt;
	bool			comp1_irq_enable;	//enable interrupt on comp1 interrupt
	uint32_t		comp1_evt;
	bool			uf_irq_enable;		//enable interrupt on ug interrupt
	uint32_t		uf_evt;
} APP_LETIMER_PWM_TypeDef ;


//***********************************************************************************
// function prototypes
//***********************************************************************************{
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct);
void LETIMER0_IRQHandler(void);
#endif
