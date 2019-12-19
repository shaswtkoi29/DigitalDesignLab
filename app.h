//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_cmu.h"
#include "em_prs.h"
#include "em_assert.h"
#include "cmu.h"
#include "gpio.h"
#include "scheduler.h"
#include <stdint.h>
#include "SI7021.h"
#include "ble.h"
#include "gpio.h"
#include "letimer.h"
#include "prs.h"
#include "adc.h"
#include "scheduler.h"
#include "retargetserial.h"
#include <string.h>
#include "sleep_routines.h"

//***********************************************************************************
// defined files
//***********************************************************************************
#define		PWM_PER				10	// PWM period in seconds // previous value 2.5
#define		PWM_ACT_PER			0.20	// PWM active period in seconds

#define		PRS_LETIMER0_CHAN	   0		// PRS channel that can go to pin PF4
#define		PRS_LETIMER0_OUTPUT	   0		// Using LETIMER0 output
//#define		PRS_CH_CTRL_SOURCESEL_LETIMER0	0x0000000EUL
#define		PRS_LETIMER0_SRC	   PRS_CH_CTRL_SOURCESEL_LETIMER0		// Locate Enumeration from SiLabs HAL document
//#define		PRS_CH_CTRL_SOURCESEL_LETIMER0	0x0000000EUL
#define		GPIO_LED0_LOC		   4		// Per PG data sheet, PRS channel 0 routed to PF4
//application scheduled events
#define 	LETIMER0_COMP0_EVT	   0x00000001
#define 	LETIMER0_COMP1_EVT	   0x00000002
#define		LETIMER0_UF_EVT		   0x00000004
#define 	SI7021_TEMP_DONE	   0x00000008
//UART
#define		BOOT_UP_EVT			   0x00000010
#define     LEUART0_TX_DONE_EVT    0x00000020
//temt photo sensor
#define		TEMT6000_SINGLE_EVT    0x00000040
#define		TEMT6000_SINGLECMP_EVT 0x00000080
#define	    Window_High_EVT		   0x00000090
#define	    Window_Low_EVT		   0x00000100
#define 	LEUART0_SIG			   0x00001000
#define		LEUART0_START		   0x00010000
//***********************************************************************************
// global variables
//***********************************************************************************

 //volatile int reg_sig ;
 volatile uint8_t x_cnt;
//***********************************************************************************
// function prototypes
//***********************************************************************************
void app_peripheral_setup(void);
void app_letimer_pwm_open(float period, float act_period);
void app_prs_pwm_open(unsigned int producer, uint8_t output, unsigned int consumer, uint8_t channel, bool async);
void scheduled_letimer0_uf_evt(void);
void scheduled_letimer0_comp0_evt(void);
void scheduled_letimer0_comp1_evt(void);
void scheduled_si7021_temp_evt(void);
void scheduled_boot_up_evt(void);
void LEUART_TX_DONE_EVT(void);
//void LEUART0_SIG_DONE(void);
//void LEUART0_START_DONE(void);
//void scheduled_TEMTtSingle_evt(void);
//void scheduled_TEMTSinglecmp_evt(void);
//void scheduled_Window_High_EVT(void);
//void scheduled_Window_Low_EVT(void);


