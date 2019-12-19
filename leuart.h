//***********************************************************************************
// Include files
//***********************************************************************************
#include "em_leuart.h"
#include "sleep_routines.h"
#include "ble.h"
#include "em_core.h"
#include <stdbool.h>

#ifndef SRC_HEADER_FILES_LEUART_H_
#define SRC_HEADER_FILES_LEUART_H_

//***********************************************************************************
// defined files
//***********************************************************************************
#define LEUART_CLOCK  	 	32768 //hz
#define	BAUDRATE	   		9600
#define leuart_data			leuartDatabits8
#define	leuart_enable		leuartEnable
#define	leuart_diable		leuartDisable
#define	leuart_parity		leuartNoParity
#define leuart_stop			leuartStopbits1
#define	leuart_reffreq		0
#define	leuart_off			0
#define	leuart_on			1
//#define leuart_tx_pin 	    12
//#define leuart_rx_pin  		14
#define	buffer_size	   		100
#define LEUART_EM_BLOCK		EM3


typedef struct {
	uint32_t				baudrate;
	LEUART_Databits_TypeDef	databits;
	LEUART_Enable_TypeDef	enable;
	LEUART_Parity_TypeDef	parity;
	uint32_t				reffreq;
	LEUART_Stopbits_TypeDef	stopbits;
	uint32_t 				LEUART_RX;
	uint32_t				LEUART_TX;
    bool 				    To_TX;
    char					ReciveBuffer[buffer_size];
    uint8_t				    n_TX;
    uint8_t 				count_TX;
} LEUART_OPEN_STRUCT;
typedef struct {
	 bool 					To_TX;
	 char					ReciveBuffer[buffer_size];
	 uint8_t 				n_TX;
	 uint8_t 				count_TX;
	 bool					state;
}LEUART_IO_STRUCT;
//***********************************************************************************
// global variables
//***********************************************************************************
LEUART_IO_STRUCT leuart_IO;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings);
void LEUART0_IRQHandler(void);
void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len);
bool leuart_tx_busy(LEUART_TypeDef *leuart);
void leuart_rx_disable(LEUART_TypeDef *leuart);
void leuart_transmit_byte(LEUART_TypeDef *leuart);

uint32_t leuart_status(LEUART_TypeDef *leuart);
void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update);
uint32_t leuart_ien_test_get(LEUART_TypeDef *leuart);
void leuart_ien_reset(LEUART_TypeDef *leuart, uint32_t ien_reg);
void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out);
uint8_t leuart_app_receive_byte(LEUART_TypeDef *leuart);
char leuart_app_receive_char_byte(LEUART_TypeDef *leuart);
#endif
