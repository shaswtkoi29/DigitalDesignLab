
/**
* @file letimer.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief Enabling PWM on an LETIMER module
* @details
* app_letimer_pwm_open() function:
* The project is structured to have a driver, letimer_pwm_open(), which will
* configure the LETIMER0 peripheral for PWM operation.
* The LETIMER_TypeDef STRUCT defines the registers/elements for each of the
* registers in the LETIMER peripheral which also provides the offset address to
* access these registers from the base address of the STRUCT
* letimer_pwm_open() function:
*  implement the their function prototypes defined in app.h, letimer.h, and prs.h
* files.
* void LETIMER0_IRQHandler(void)
* handles LETIMER0 interrupt and stated the schedule and sleep


* @return
* this function handles interrupt and stores/ assign pins/ register to Letimer 0. No return
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
#include "letimer.h"
#include "scheduler.h"



//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************
static uint32_t scheduled_comp0_evt;
static uint32_t scheduled_comp1_evt;
static uint32_t scheduled_uf_evt;

//***********************************************************************************
// functions
//***********************************************************************************
/***************************************************************************//**
 * @brief
 *	Enabling PWM on an LETIMER module
 *
 * @details
 * 	Enable the letimer Clock and use an assert statement to see to check if
 * 	those were 	properly enabled
 *
 * @param letimer
 * 	 LETIMER module
 *
 * @param app_letimer_struct
 * 	configuration of the letimer module
 *
 ******************************************************************************/
void letimer_pwm_open(LETIMER_TypeDef *letimer, APP_LETIMER_PWM_TypeDef *app_letimer_struct){

		LETIMER_Init_TypeDef	letimer_pwm_values;
		if(letimer==LETIMER0){
			CMU_ClockEnable (cmuClock_LETIMER0,true);
		}
		else {
		CMU_ClockEnable (cmuClock_LETIMER0,false);
		}
		//LETIMER_COMP0_COMP0_DEFAULT (_LETIMER_COMP0_COMP0_DEFAULT<<0);
		//LETIMER_COMP1_COMP1_DEFAULT (_LETIMER_COMP1_COMP1_DEFAULT<<0);
		letimer_pwm_values.bufTop = false;
	    letimer_pwm_values.comp0Top =true;
		letimer_pwm_values.debugRun =app_letimer_struct->debugRun;
		letimer_pwm_values.enable =app_letimer_struct->enable;
		letimer_pwm_values.out0Pol=_LETIMER_CTRL_OPOL0_DEFAULT;
		letimer_pwm_values.out1Pol=_LETIMER_CTRL_OPOL1_DEFAULT;
		letimer_pwm_values.repMode =letimerRepeatFree;
		letimer_pwm_values.ufoa0=letimerUFOAPwm;
		letimer_pwm_values.ufoa1=letimerUFOAPwm;
		//letimer_pwm_values.repMode=letimerRepeatFree;
	    letimer->REP0 =0xFF;
	    //letimer->REP1 =0xFF;
		//->CTRL |=3; //UFAOA0 PIN
	    letimer->ROUTEPEN &= ~_LETIMER_ROUTEPEN_MASK;
	    letimer->ROUTEPEN |= app_letimer_struct->out_pin_0_en;
	    letimer->ROUTEPEN |= app_letimer_struct->out_pin_0_en <<1;

	    letimer->ROUTELOC0 &= ~_LETIMER_ROUTELOC0_MASK;
	    letimer->ROUTELOC0 |= app_letimer_struct->out_pin_route0;
	    letimer->ROUTELOC0 |= app_letimer_struct->out_pin_route1;

		//letimer->ROUTEPEN |=app_letimer_struct->out_pin_0_en | app_letimer_struct->out_pin_1_en ; // SET OUTPUTPIN1


		//period_cnt=app_letimer_struct->period*LETIMER_HZ;
		LETIMER_Init(letimer,&letimer_pwm_values);
		letimer->COMP0 =app_letimer_struct->period*LETIMER_HZ;
		letimer->COMP1 =app_letimer_struct->active_period*LETIMER_HZ;
		//LETIMER0->CMD |=LETIMER_CMD_CTO0;
		if((letimer->IF & 0x01)==0){
			letimer->IFS = 0x01;
			EFM_ASSERT(letimer->IF & 0x01);
			letimer->IFC = 0x01;
		}
		else {
			letimer->IFC=0x01;
			EFM_ASSERT(!(letimer->IF & 0x01));
		}
		if(app_letimer_struct->comp0_irq_enable== true){
			LETIMER0->IEN=LETIMER_IF_COMP0;
		}
		 if(app_letimer_struct->comp1_irq_enable== true){
					LETIMER0->IEN=LETIMER_IF_COMP1;
				}
		 if(app_letimer_struct->uf_irq_enable== true){
					LETIMER0->IEN=LETIMER_IF_UF;
				}
		//LETIMER0->IEN= app_letimer_struct->comp0_irq_enable | app_letimer_struct->comp1_irq_enable | app_letimer_struct->uf_irq_enable;
		//LETIMER0->IF = app_letimer_struct->comp0_evt | app_letimer_struct->comp1_evt | app_letimer_struct->uf_evt;
		NVIC_EnableIRQ(LETIMER0_IRQn);
		scheduled_comp0_evt=app_letimer_struct->comp0_evt;
		scheduled_comp1_evt=app_letimer_struct->comp1_evt;
		scheduled_uf_evt= app_letimer_struct->uf_evt;
		//LETIMER_Enable(letimer,app_letimer_struct->enable);
		//get_scheduled_events();
}
/***************************************************************************//**
 * @brief
 *	LETIMER0 IRQ Handler
 *
 * @details
 * 	schedule different  events according to the schedule event that happened
 *
 ******************************************************************************/
void LETIMER0_IRQHandler(void){
		int int_flag;
		__disable_irq();
		int_flag=LETIMER0->IF & LETIMER0->IEN;
		LETIMER0->IFC=int_flag;
		if(int_flag & scheduled_comp0_evt){
				EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP0));
				void LETIMER0_ISR(void);
				add_scheduled_event(LETIMER_IF_COMP0);

		}
		if(int_flag & scheduled_comp1_evt){
				EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_COMP1));
				void LETIMER0_ISR(void);
				add_scheduled_event(LETIMER_IF_COMP1);
		}
		if(int_flag & scheduled_uf_evt){
				EFM_ASSERT(!(LETIMER0->IF & LETIMER_IF_UF));
				void LETIMER0_ISR(void);
				add_scheduled_event(LETIMER_IF_UF);
		}
		__enable_irq();
}


