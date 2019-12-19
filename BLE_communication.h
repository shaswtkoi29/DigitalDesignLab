/*
 * BLE_communication.h
 *
 *  Created on: Nov 21, 2019
 *      Author: shaswat
 */

#ifndef SRC_HEADER_FILES_BLE_COMMUNICATION_H_
#define SRC_HEADER_FILES_BLE_COMMUNICATION_H_



//** Standard Libraries
#include <stdbool.h>
#include "app.h"
#include "leuart.h"
#include "i2c.h"
#include "Si7021.h"




//***********************************************************************************
// defined files
//***********************************************************************************
#define BLE_COMM_STARTFRAME			0x12  //bttn 1
#define BLE_COMM_SIGFRAME			0x14 //bttn 2
#define F_COMMAND					1
#define	C_COMMAND					2
#define F_READ						0x00
#define C_READ						0x01




//***********************************************************************************
// global variables
//***********************************************************************************
 char show_value[8];
volatile int reg_sig;
//volatile bool stat_tx	= false;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void BLE_communication_test(LEUART_TypeDef * leuart);
void enable_leuart_status(void);
void disable_leuart_status(void);
void temp_display(LEUART_TypeDef * leuart);
void temp_display_F(float temp_fahrenheit);
void temp_display_C(float temp_celsius);
void int_to_string(int n,int s,char *c);
int BLE_return(char *show_val);

#endif /* SRC_HEADER_FILES_BLE_COMMUNICATION_H_ */
