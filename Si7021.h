
#ifndef SRC_HEADER_FILES_SI7021_H_
#define SRC_HEADER_FILES_SI7021_H_
//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"
#include "gpio.h"
#include "i2c.h"
#include <stdbool.h>

//***********************************************************************************
// defined files
//***********************************************************************************
#define	SI7021_SCL_PORT			 gpioPortC		//PC11
#define	SI7021_SCL_PIN			 11		     //PIN# FROM SI7021 DATA SHEET
#define	SI7021_SDA_PORT			 gpioPortC		//PC10
#define	SI7021_SDA_PIN			 10    //PIN# FROM SI7021 DATA SHEET
#define	SI7021_SENSOR_EN_PORT	 gpioPortB		//PB10
#define	SI7021_SENSOR_EN_PIN	 10

//
#define Si7021_dev_addr			0x40
#define Si7021_TEMP_NO_HOLD		0xF3
#define Si7021_I2C_FREQ			400000		//hz
#define Si7021_I2C_CLK_RATIO	i2cClockHLRFast
#define Si7021_SCL_LOC			15
#define Si7021_SCL_EN			2
#define Si7021_SDA_LOC			15
#define Si7021_SDA_EN			1
#define Si7021_I2C				I2C0
#define Si7021_I2C_FREQ_MAX		400000


//***********************************************************************************
// global variables
//***********************************************************************************
I2C_IO_STRUCT i2cIO;
I2C_OPEN_STRUCT i2cOpen;

//***********************************************************************************
// function prototypes
//***********************************************************************************

void si7021_i2c_open();
void si7021_read(void);
void si7021_enable(void);
void si7021_disable(void);
float si7021_measure_temp(void);
float si7021_measure_temp_celsius(void);
#endif
