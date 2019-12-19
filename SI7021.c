/**
* @file SI7021.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief Configures an I2C port to communicate to the SI7021 sensor to calculate the temperature
* @details
*  communicate with I2C.c to transmit the data read from Si7021 sensor, using struct function
*/
//***********************************************************************************
// Include files
//***********************************************************************************

#include "i2c.h"
#include "Si7021.h"
#include <stdint.h>
#include <stdbool.h>

static uint16_t temp;


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Configures an I2C port to communicate with SI7021 sensor
 *
 * @details
 *  Configure the necessary element to open the I2C port
 *  for communication with SI7021
 *
 ******************************************************************************/
void si7021_i2c_open(void){
	i2cIO.SDA_PORT=SI7021_SDA_PORT;
	i2cIO.SCL_PORT=SI7021_SCL_PORT;
	i2cIO.SCL_PIN=SI7021_SCL_PIN;
	i2cIO.SDA_PIN=SI7021_SDA_PIN;

	//Declaring value for i2cOpen
	i2cOpen.Clhr		=i2cClockHLRStandard;
	i2cOpen.enable		=true;
	i2cOpen.freq		=Si7021_I2C_FREQ_MAX;
	i2cOpen.master		=true;
	i2cOpen.refFreq		=0;
	i2cOpen.SDALOC		=Si7021_SDA_LOC;
	i2cOpen.SCLLOC		=Si7021_SCL_LOC;
	i2c_Open(Si7021_I2C,i2cOpen,i2cIO);
}
/***************************************************************************//**
 * @brief
 *	Enable the SI7021 device
 *
 * @details
 *  turn on the device by enabling EN pin on the SI7021
 *
 ******************************************************************************/
void si7021_enable (void){
	GPIO_PinOutSet(SI7021_SENSOR_EN_PORT,SI7021_SENSOR_EN_PIN);
}

/***************************************************************************//**
 * @brief
 *	Disable the SI7021 device
 *
 * @details
 *   turn off the device by disabling EN pin on the SI7021
 *
 ******************************************************************************/
void si7021_disable (void){
	GPIO_PinOutClear(SI7021_SENSOR_EN_PORT,SI7021_SENSOR_EN_PIN);
}
/***************************************************************************//**
 * @brief
 *	read the value from the SI7021 sensor through I2C
 *
 * @details
 *  Initialize I2C state machine to take a measurement and read the value from
 *  the SI7021 sensor
 *
 ******************************************************************************/
void si7021_read(void){
	i2c_start(Si7021_I2C,Si7021_dev_addr,Si7021_TEMP_NO_HOLD,I2C_Read,&temp);
}
/***************************************************************************//**
 * @brief
 *	Schedules the SI7021 temperature measurement  event
 *
 * @details
 * Mathematics to convert the measured value to fahrenheit
 *
 * @return
 * returns temperature in fahrenheit
 ******************************************************************************/
float si7021_measure_temp(void){
	float temp_in_celsius = (((175.72 * temp) / 65536) - 46.85);
	float temp_fahrenheit =(temp_in_celsius* 1.8) + 32;
	return temp_fahrenheit;  //return temp in F

}
/***************************************************************************//**
 * @brief
 *	Schedules the SI7021 temperature measurement  event
 *
 * @details
 * Mathematics to convert the measured value to celsius
 *
 * @return
 * returns temperature in celsius
 ******************************************************************************/
float si7021_measure_temp_celsius(void){
	float temp_in_celsius = (((175.72 * temp) / 65536) - 46.85);
	return temp_in_celsius;  //return temp in C
}
