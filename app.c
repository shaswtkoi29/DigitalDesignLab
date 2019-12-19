/**
* @file app.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief
* the app.c file, the arguments to the function calls within the
* app_peripheral_setup.c function.These functions will be used to set up the structures that will be used by the
* peripheral’s open driver functions.
* @details
* this function assigns values to letimer, schedule, sleep_routine, prs and call those functions
*
*
* @return
* no return; calls functions
*
*
******************************************************************************/
//***********************************************************************************
// Include files
//***********************************************************************************
#include "app.h"
#include "BLE_communication.h"


//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************
 bool flag = false;


//static bool window_stat_bool = true;
//***********************************************************************************
// function
//***********************************************************************************
//LETIMER_TypeDef *letimer;
APP_LETIMER_PWM_TypeDef app_letimer_struct;
APP_PRS_CHAN_TypeDef prs_setup;
/***************************************************************************//**
 * @brief
 *	Call the open peripheral functions
 *
 * @details
 * 	CMU, GPIO,SI7021,LEUART,schecule, BLE, I2C, and LETIMER are enabled
 *
 ******************************************************************************/
void app_peripheral_setup(void){
	add_scheduled_event(BOOT_UP_EVT);
	cmu_open();
	gpio_open();
	si7021_i2c_open();
	ble_open();

//  temt6000_open();
	app_letimer_pwm_open(PWM_PER,PWM_ACT_PER );

}
/***************************************************************************//**
 * @brief
 *	Configure an LETIMER module for PWM
 *
 * @details
 *	Configure which pins should be output to and what interrupts should be used
 *
 * @param period
 *	Desired period that the PWM will operate at in second
 *
 * @param act_period
 * 	active period of PWM period
 *
 ******************************************************************************/
void app_letimer_pwm_open(float period, float act_period){
	// Initializing LETIMER0 for PWM open function


	    app_letimer_struct.period=period;
		app_letimer_struct.active_period=act_period;
		app_letimer_struct.debugRun=false;
		app_letimer_struct.enable=false;
		app_letimer_struct.out_pin_0_en=!PRS_LETIMER0_OUTPUT;
		app_letimer_struct.out_pin_1_en=PRS_LETIMER0_OUTPUT;
		app_letimer_struct.uf_irq_enable=true;
		app_letimer_struct.comp0_irq_enable=false;
		app_letimer_struct.comp1_irq_enable=false;
		app_letimer_struct.comp0_evt=LETIMER0_COMP0_EVT;
		app_letimer_struct.comp1_evt=LETIMER0_COMP1_EVT;
		app_letimer_struct.uf_evt=LETIMER0_UF_EVT;
		if(PRS_LETIMER0_OUTPUT==0) {app_letimer_struct.out_pin_route0=GPIO_LED0_LOC;}
		else {app_letimer_struct.out_pin_route1=GPIO_LED0_LOC;}
		letimer_pwm_open(LETIMER0,&app_letimer_struct);

}
/***************************************************************************//**
 * @brief
 *  Configure the PRS peripheral to output a PWM signal
 *
 * @details
 *	Configure a PRS channel to output the LETIMER's PWM signal
 *
 * @param producer
 * A source to select for the channel
 *
 * @param output
 * 	Where the producer signal should be output to
 *
 * @param consumer
 *	The signal being output
 *
 * @param channel
 *	A channel to define the source and signal
 *
 * @param async
 * 	Choose asynchronous or synchronous
 ******************************************************************************/
void app_prs_pwm_open(unsigned int producer,uint8_t output, unsigned int consumer, uint8_t channel, bool async){

		prs_setup.async=async;
		prs_setup.chan_number=channel;
		prs_setup.route_enable=true;
		prs_setup.route_loc=output;
		prs_setup.signal=consumer;
		prs_setup.source=producer;
		//prs_pwm_open(&prs_setup);
}
/***************************************************************************//**
 * @brief
 *	Schedules the LETIMER0 UF event and selects Low-Power Mode
 *
 * @details
 *	Makes sure that the LETIMER UF event is scheduled and removes it from the list,
 *	reads the SI7021
 *
 ******************************************************************************/
void scheduled_letimer0_uf_evt(void){
	EFM_ASSERT(get_scheduled_events() & LETIMER0_UF_EVT);
	remove_scheduled_event(LETIMER0_UF_EVT);
	si7021_read();
}
/***************************************************************************//**
 * @brief
 *	Schedules the LETIMER0 comp0 event and selects Low-Power Mode
 *
 * @details
 *	Makes sure that the comp 0 event is scheduled and removes it from the list,
 *	Decides a sleep mode to utilize based on the previously used sleep mode
 *  Checks that the event was scheduled and then clears the event from the
 *	events flag
 ******************************************************************************/
void scheduled_letimer0_comp0_evt(void){
	remove_scheduled_event(LETIMER0_COMP0_EVT);
	EFM_ASSERT(false);
	//EFM_ASSERT(get_scheduled_events() & LETIMER0_COMP0_EVT);
}
/***************************************************************************//**
 * @brief
 *	Schedules the LETIMER0 comp1 event and selects Low-Power Mode
 *
 * @details
 *	Makes sure that the comp 1 event is scheduled and removes it from the list,
 *	Checks that the event was scheduled and then clears the event from the
 *	events flag
 *
 ******************************************************************************/
void scheduled_letimer0_comp1_evt(void){
	remove_scheduled_event(LETIMER0_COMP1_EVT);
	EFM_ASSERT(false);
	//EFM_ASSERT(get_scheduled_events() & LETIMER0_COMP1_EVT);

}
/***************************************************************************//**
 * @brief
 *	Schedules the SI7021 TEMP DONE event
 *
 * @details
 *  Makes sure that the SI7021 event is scheduled and removes it from the list,
 *  then  wakes up the device by enabling LEUART status , GPIO pins and I2c BUS
 *  that has been in sleep mode so that we can collect data from SI7021 and
 *  after calculation and display it disables  LEUART status GPIO pins, disable
 *  the SI7021 and unlocking the sleep mode.
 *  This also the value that was written to the si7021.c private variable and
 *  converts that value to Celsius and then Fahrenheit. After these conversions
 *  we check the condition (command from BLE terminal app) to display accordingly
 *
 ******************************************************************************/
void scheduled_si7021_temp_evt(void){
	EFM_ASSERT(get_scheduled_events() & SI7021_TEMP_DONE);
	remove_scheduled_event(SI7021_TEMP_DONE);
	enable_leuart_status();
    temp_display(HM10_LEUART0);
    disable_leuart_status();
    LEUART0->IEN    			|=		LEUART_IEN_TXBL;
    leuart_IO.To_TX    	         =		false;
}
/***************************************************************************//**
 * @brief
 *	Handles the BOOT_UP event
 *
 * @details
 * 	Makes sure that the  boot up event is scheduled and removes it from the list,
 *	Writes "Hello World" over BLE communication
 *	and the setup BLE communication test which is assigning SIGFRAME
 *	and STARTFRAME so it goes through assert check to verify stored value
 *
 ******************************************************************************/
void scheduled_boot_up_evt(void){
	EFM_ASSERT(get_scheduled_events() & BOOT_UP_EVT);
	remove_scheduled_event(BOOT_UP_EVT);
	ble_write("\nHello World!\n");
    BLE_communication_test(HM10_LEUART0);
	//circular_buff_test();
	//ble_write("\nHello World!\n");
	//ble_write("ADC Lab\n");
	//ble_write("Shaswat Koirala\n");

}
/***************************************************************************//**
 * @brief
 *	Handles the LEUART TX DONE event
 *
 * @details
 * 	Makes sure that the  tx done event is scheduled and removes it from the list,
 *  enables and disable interrupt depending on amount of bits that needs to be send
 ******************************************************************************/
void LEUART_TX_DONE_EVT(void){
	EFM_ASSERT(get_scheduled_events() & LEUART0_TX_DONE_EVT);
	remove_scheduled_event(LEUART0_TX_DONE_EVT);
	if(leuart_IO.To_TX==false)	{
		   LEUART0->IEN    &=		~LEUART_IEN_TXBL;
	}
	else{
		 LEUART0->IEN    |=			 LEUART_IEN_TXBL;
	}
    //ble_circ_pop(false);
}





