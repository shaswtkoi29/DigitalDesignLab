/**
* @file ble.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief Configure LEUART for BLE
* @deread_ptrs
*	Opens LEUART0 to be used with the HM10 BLE module at 9600,
*/
//***********************************************************************************
// Include files
//***********************************************************************************
#include "ble.h"
#include "leuart.h"
#include "BLE_communication.h"


//***********************************************************************************
// defined files
//***********************************************************************************
#include <string.h>
#include <stdlib.h>
#include "em_assert.h"

//***********************************************************************************
// private variables
//***********************************************************************************
static CIRC_TEST_STRUCT test_struct;
static BLE_CIRCULAR_BUF ble_buff;

//***********************************************************************************
// prototypes
//***********************************************************************************

void ble_open(void){
	//ble_circ_init();
	LEUART_OPEN_STRUCT leuart_timer;
	leuart_timer.baudrate = BAUDRATE;
	leuart_timer.parity = leuartNoParity;
	leuart_timer.stopbits = leuartStopbits1;
	leuart_timer.databits = leuartDatabits8;
	leuart_timer.enable = leuartEnable;
	leuart_timer.reffreq = 0;
	leuart_timer.LEUART_RX = UART_RX_LOC;
	leuart_timer.LEUART_TX = UART_TX_LOC;
	leuart_open(HM10_LEUART0, &leuart_timer);
	//BLE_communication_test(HM10_LEUART0);
}


/***************************************************************************//**
 * @brief
 *	Send string over BLE
 *
 * @details
 * 	Takes a string and sends it over leaurt_start
 *
 * @param string
 *	String desired to be sent over BLE
 ******************************************************************************/
void ble_write(char *string){
	uint8_t	length 	=	strlen(string);
	leuart_start(HM10_LEUART0,string,length);
}

/***************************************************************************//**
  * @detail
  * following code is to test BLE to that you can change the bluetooth module name
  * following code consist of ble_write using push and pop which helps display more than
  * command


void ble_circular_buff_write(char* string){
	ble_circ_push(string);
	ble_circ_pop(false);
}
void ble_circ_push(char *string) {
	uint8_t str_length = strlen(string);
	if(ble_circ_space() >= str_length) {  // add to buffer

		// write string's length to the buffer
		ble_buff.buff[ble_buff.write_ptr] = str_length;
		update_circ_wrtindex(&ble_buff, 1); // increment

		// write string's characters to the buffer
		int i;
		for(i = 0; i < str_length; i++) {
			char added_char = string[i];
			ble_buff.buff[ble_buff.write_ptr] = added_char;// string[i]; // add
			update_circ_wrtindex(&ble_buff, 1); // increment
		}
	}
	else {  // not enough room in buffer
		EFM_ASSERT(false);
	}
}


bool ble_circ_pop(bool test) {
	// check if there's anything to pop
	if(!(ble_circ_space() < ble_buff.buffsize)) {  // returns true if there is nothing
		return true;
	}

	// check if currently transmitting
	if(leuart_tx_busy(HM10_LEUART0) == true) {
		return true;
	}

	// now do popping
	if(test == false) {
		// read how many bytes to read/pop
		char str_length = ble_buff.buff[ble_buff.read_ptr];
		update_circ_readindex(&ble_buff, 1);

		char temp_str[str_length];  // temporary string for transmission

		int i;
		for(i = 0; i < str_length; i++) {
			char popped = ble_buff.buff[ble_buff.read_ptr];
			temp_str[i] = popped; // ble_buff.buff[ble_buff.read_ptr];
			update_circ_readindex(&ble_buff, 1);
		}
		leuart_start(HM10_LEUART0, temp_str, str_length);
	}

	else { // test = true
//		 read how many bytes to read/pop
		uint8_t str_length = ble_buff.buff[ble_buff.read_ptr];
		update_circ_readindex(&ble_buff, 1);

		int i;
		for(i = 0; i < str_length; i++) {
			test_struct.result_str[i] = ble_buff.buff[ble_buff.read_ptr];
			update_circ_readindex(&ble_buff, 1);
		}
	}
	return false;
}


uint8_t ble_circ_space(void) {
	return (ble_buff.buffsize - ble_buff.size);
}

void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by) {
	if((index_struct->write_ptr + update_by) > (index_struct->buffsize - 1)) {
		index_struct->write_ptr = 0;
	}
	else {
		index_struct->write_ptr += update_by;
	}
	index_struct->size += update_by;
}


void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by) {
	if((index_struct->read_ptr + update_by) > (index_struct->buffsize -1)) {
		index_struct->read_ptr = 0;
	}
	else {
		index_struct->read_ptr += update_by;
	}
	index_struct->size -= update_by;
}


bool ble_test(char *mod_name){
	uint32_t	str_len;

	// This test will limit the test to the proper setup of the LEUART
	// peripheral, routing of the signals to the proper pins, pin
	// configuration, and transmit/reception verification.  The test
	// will communicate with the BLE module using polling routines
	// instead of interrupts.
	// How is polling different than using interrupts?
	// ANSWER: Interrupts allow for event-driven code, meaning that we can check/process
	// 			other things while waiting for an event occur. That event also may never
	//			occur, so by using polling the device would get stuck in an infinite loop

	// How does interrupts benefit the system for low energy operation?
	// ANSWER: We can go to sleep in between interrupt events.

	// How does interrupts benefit the system that has multiple tasks?
	// ANSWER: We can do pieces of other tasks while waiting for an event to occur

	// First, you will need to review the DSD HM10 datasheet to determine
	// what the default strings to write data to the BLE module and the
	// expected return statement from the BLE module to test / verify the
	// correct response

	// The break_str is used to tell the BLE module to end a Bluetooth connection
	// such as with your phone.  The ok_str is the result sent from the BLE module
	// to the micro-controller if there was not active BLE connection at the time
	// the break command was sent to the BLE module.
	// Replace the break_str "" with the command to break or end a BLE connection
	// Replace the ok_str "" with the result that will be returned from the BLE
	//   module if there was no BLE connection
	char		break_str[80] = "AT";
	char		ok_str[80] = "OK";

	// output_str will be the string that will program a name to the BLE module.
	// From the DSD HM10 datasheet, what is the command to program a name into
	// the BLE module?
	// The  output_str will be a string concatenation of the DSD HM10 command
	// and the input argument sent to the ble_test() function
	// Replace the output_str "" with the command to change the program name
	// Replace the result_str "" with the first part of the expected result
	//  the backend of the expected response will be concatenated with the
	//  input argument
	char		output_str[80] = "AT+NAME";
	char		result_str[80] = "OK+Set:";

	// To program the name into your module, you must reset the module after you
	// have sent the command to update the modules name.  What is the DSD HM10
	// name to reset the module?
	// Replace the reset_str "" with the command to reset the module
	// Replace the reset_result_str "" with the expected BLE module response to
	//  to the reset command
	char		reset_str[80] = "AT+RESET";
	char		reset_result_str[80] = "OK+RESET";
	char		return_str[80];

	bool		success;
	bool		rx_disabled, rx_en, tx_en;
	uint32_t	status, leuart_ien;

	// These are the routines that will build up the entire command and response
	// of programming the name into the BLE module.  Concatenating the command or
	// response with the input argument name
	strcat(output_str, mod_name);
	strcat(result_str, mod_name);

	// The test routine must not alter the function of the configuration of the
	// LEUART driver, but requires certain functionality to insure the logical test
	// of writing and reading to the DSD HM10 module.  The following c-lines of code
	// disable the interrupt handler and save the current state of the LEUART driver
	// that will be used later to re-install the LEUART configuration

	leuart_ien = leuart_ien_test_get(HM10_LEUART0);
	status = leuart_status(HM10_LEUART0);
	if (status & LEUART_STATUS_RXBLOCK) {
		rx_disabled = true;
		// Enabling, unblocking, the receiving of data from the LEUART RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKDIS);
	}
	else rx_disabled = false;
	if (status & LEUART_STATUS_RXENS) {
		rx_en = true;
	} else {
		rx_en = false;
		// Enabling the receiving of data from the RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_RXENS));
	}

	if (status & LEUART_STATUS_TXENS){
		tx_en = true;
	} else {
		// Enabling the transmission of data to the TX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_TXENS));
		tx_en = false;
	}
	leuart_cmd_write(HM10_LEUART0, (LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX));

	// This sequence of instructions is sending the break ble connection
	// to the DSD HM10 module.
	// Why is this command required if you want to change the name of the
	// DSD HM10 module?
	// ANSWER: The device connected to the HM10 module will likely not recognize
	// 			the HM10

	str_len = strlen(break_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, break_str[i]);
	}

	// What will the ble module response back to this command if there is
	// a current ble connection?
	// ANSWER:	OK+LOST

	str_len = strlen(ok_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (ok_str[i] != return_str[i]) {
				EFM_ASSERT(false);
		}
	}

	// This sequence of code will be writing or programming the name of
	// the module to the DSD HM10
	str_len = strlen(output_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, output_str[i]);
	}

	// Here will be the check on the response back from the DSD HM10 on the
	// programming of its name
	str_len = strlen(result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (result_str[i] != return_str[i]) {
			EFM_ASSERT(false);
		}
	}

	// It is now time to send the command to RESET the DSD HM10 module
	str_len = strlen(reset_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, reset_str[i]);
	}

	// After sending the command to RESET, the DSD HM10 will send a response
	// back to the micro-controller
	str_len = strlen(reset_result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (reset_result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// After the test and programming have been completed, the original
	// state of the LEUART must be restored
	if (!rx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXDIS);
	if (rx_disabled) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKEN);
	if (!tx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXDIS);
	leuart_ien_reset(HM10_LEUART0, leuart_ien);

	success = true;

	return success;
}


void circular_buff_test(void){
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response:  point to the same location in the buffer


	 ble_buff.write_ptr = 0;
	 ble_buff.read_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response: NULL character is 0 in decimal
	 //
	 for (int i = 0;i < test1_len; i++){
		 test_struct.test_str[0][i] = i+1;
	 }
	 for (int i = 0;i < test2_len; i++){
		 test_struct.test_str[1][i] = i + 20;
	 }
	 for (int i = 0;i < test3_len; i++){
		 test_struct.test_str[2][i] = i +  35;
	 }

	 // Why is there only one push to the circular buffer at this stage of the test
	 // Student Response:pushing to the  work
	 //
	 ble_circ_push(&test_struct.test_str[0][0]);

	 // Why is the expected buff_empty test = false?
	 // Student Response: if  popped then the function should give false

	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What does this next push on the circular buffer test?
	 // Student Response: write to the proper index

	 ble_circ_push(&test_struct.test_str[1][0]);

	 // What does this next push on the circular buffer test?
	 // Student Response:  can write more than once to the buffer
	 ble_circ_push(&test_struct.test_str[2][0]);


	 // Why is the expected buff_empty test = false?
	 // Student Response: if  popped then the function should give false
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // Why is the expected buff_empty test = false?
	 // Student Response: There are more items in the buff that weren't
	 //		already popped
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response:
	 // 	- write a string, see if we can pop the entire string,
	 // 	and see if we can still write to it
	 // 	- See how writing to the buffer works when its already full


	 // Why is the expected buff_empty test = true?
	 // Student Response: There should be nothing in the buffer so
	 // 	the pop function will fail
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n");
}
******************************************************************************/
