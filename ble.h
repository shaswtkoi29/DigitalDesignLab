
//***********************************************************************************
// Include files
//***********************************************************************************
#ifndef SEC_HEADER_FILES_ble_H_
#define SEC_HEADER_FILES_ble_H_
//** Standard Libraries
#include <stdbool.h>
#include <app.h>


//***********************************************************************************
// defined files
//***********************************************************************************
#define HM10_LEUART0		LEUART0
#define	UART_RX_Port		gpioPortD
#define	UART_RX_Pin			11
#define UART_RX_LOC			18
#define	UART_TX_Port		gpioPortD
#define	UART_TX_Pin			10
#define UART_TX_LOC			18
#define CIRC_TEST_SIZE		3
#define DATA_SIZE			64
//#define BLE_SIZE			64
#define CIRC_TEST			true

//typedef struct {
//
//} BLE_IO_STRUCT;

//***********************************************************************************
// global variables
//***********************************************************************************
typedef struct {
	char test_str[CIRC_TEST_SIZE][64];
	char result_str[64];
}CIRC_TEST_STRUCT;

typedef struct{
	char *buff;
	char	data[DATA_SIZE];
	uint8_t overflowbuff;
	int buffsize;
	volatile uint8_t size;
	volatile uint8_t read_ptr;
	volatile uint8_t write_ptr;
	bool test;
}BLE_CIRCULAR_BUF;

//***********************************************************************************
// function prototypes
//***********************************************************************************
void ble_open(void);
//void ble_leuart_gpio(BLE_IO_STRUCT *io_pins);
void ble_write(char *string);
void ble_circular_buff_write(char *string);

bool ble_test(char *mod_name);
bool ble_test(char *mod_name);
void ble_circ_init(void);
void ble_circ_push(char *string);
bool ble_circ_pop(bool test);
uint8_t ble_circ_space(void);
void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);
void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct,uint32_t update_by);
void circular_buff_test(void);
#endif
