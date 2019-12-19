/**
* @file gpio.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief enable GPIO pins and clock assigning them according to peripheral used
* @details
*  enable GPIO pins and clock assigning them according to peripheral used
*/
//***********************************************************************************
// Include files
//***********************************************************************************
#include "gpio.h"
#include "ble.h"
#include "em_cmu.h"
#include "Si7021.h"
//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************


//***********************************************************************************
// functions
//***********************************************************************************

void gpio_open(void){

	CMU_ClockEnable(cmuClock_GPIO, true);

	// Set LED ports to be standard output drive with default off (cleared)
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);

	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, LED0_default);


	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
//	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, LED1_default);
	//Si7021 sensoR
	GPIO_DriveStrengthSet(SI7021_SENSOR_EN_PIN, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(SI7021_SENSOR_EN_PORT, SI7021_SENSOR_EN_PIN, gpioModePushPull, true);
	//operating logic AND on I2C pin
	GPIO_PinModeSet(SI7021_SCL_PORT,SI7021_SCL_PIN,gpioModeWiredAnd,0);
	GPIO_PinModeSet(SI7021_SDA_PORT,SI7021_SDA_PIN,gpioModeWiredAnd,0);
	//UART-TX

	GPIO_DriveStrengthSet(UART_TX_Pin, gpioDriveStrengthStrongAlternateWeak);
	GPIO_PinModeSet(UART_TX_Port, UART_TX_Pin, gpioModePushPull, 1);
	//UART-RX
	GPIO_PinModeSet(UART_RX_Port, UART_RX_Pin, gpioModeInput, 0);
	// TEMPT6000
	//GPIO_PinModeSet (TEMT6000_PORT,TEMT6000_PIN,gpioModeInput,0);

}
