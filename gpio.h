//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_gpio.h"

//***********************************************************************************
// defined files
//***********************************************************************************

// LED0 pin is
#define	LED0_port		gpioPortF
#define LED0_pin		04u
#define LED0_default	false 	// off
// LED1 pin is
#define LED1_port		gpioPortF
#define LED1_pin		05u
#define LED1_default	false	// off

//SI7021
#define	SI7021_SCL_PORT			 gpioPortC		//PC11
#define	SI7021_SCL_PIN			 11		     //PIN# FROM SI7021 DATA SHEET
#define	SI7021_SDA_PORT			 gpioPortC		//PC10
#define	SI7021_SDA_PIN			 10    //PIN# FROM SI7021 DATA SHEET
#define	SI7021_SENSOR_EN_PORT	 gpioPortB		//PB10
#define	SI7021_SENSOR_EN_PIN	 10

//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void gpio_open(void);

