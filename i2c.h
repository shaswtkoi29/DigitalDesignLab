/*
 * i2c.h
 *
 *  Created on: Oct 8, 2019
 *      Author: shaswat
 */

//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SEC_HEADER_FILES_I2C_H_
#define SEC_HEADER_FILES_I2C_H_
//***********************************************************************************
// defined files
//***********************************************************************************
#include <stdint.h>
#include "gpio.h"
//#include "em_i2c.c"
#include "sleep_routines.h"
#include "em_i2c.h"
//#define SI7021_FREQ 	400000 //frequency in hz
//#define REF_FREQ		0
#define i2c_read		1
#define	i2c_write		0
#define I2C_EM_BLOCK	EM2

//***********************************************************************************
// global variables
//***********************************************************************************
//typedef struct {
//
//} I2C_INIT_TypeDef ;
typedef struct {
		I2C_ClockHLR_TypeDef	Clhr;
		bool					enable;
		uint32_t				freq;
		bool					master;
		uint32_t				refFreq;
	//	I2C_Init_TypeDef		I2CInit;
//		bool					SCLPEN;
//		bool					SDAPEN;
		uint32_t				SDALOC;
		uint32_t				SCLLOC;
} I2C_OPEN_STRUCT ;

typedef struct {
	uint32_t	SCL_PIN;
	uint32_t	SDA_PIN;
	uint32_t	SDA_PORT;
	uint32_t	SCL_PORT;

}I2C_IO_STRUCT;

typedef enum {
	Start,
	RegSend,
	Re_Start,
	SlaveData,
	Stop
} I2C_STATE_MACHINE;

typedef enum {
	I2C_Read = 0,
	I2C_Write = 1
} I2C_Read_Write;


typedef struct {
	I2C_TypeDef *i2c;
	I2C_STATE_MACHINE SystemState;
	I2C_Read_Write RW;
	uint8_t DeviceAdr;
	uint8_t RegAdr;
	uint16_t *buffer;
} I2C_PAYLOAD_STRUCT;





//***********************************************************************************
// function prototypes
//***********************************************************************************
void i2c_Open(I2C_TypeDef *i2c,I2C_OPEN_STRUCT i2cOpen,I2C_IO_STRUCT i2cIO);
void i2c_bus_reset(I2C_TypeDef *i2c,I2C_IO_STRUCT i2cIO);
void i2c_start(I2C_TypeDef *i2c,uint8_t SlaveDevice, uint8_t RegAddr, I2C_Read_Write RW, uint16_t *buffer);
void I2C_NACK(void);
void I2C_ACK(void);
void I2C_RXDATAV(void);

#endif
