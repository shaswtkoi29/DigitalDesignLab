/**
* @file leuart.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief configures a LEUART peripheral
*/

//***********************************************************************************
// Include files
//***********************************************************************************

//** Standard Library includes
#include <string.h>

//** Silicon Labs include files
#include "em_gpio.h"
#include "em_cmu.h"

//** Developer/user include files
#include "leuart.h"
#include "app.h"
#include "scheduler.h"
#include "BLE_communication.h"
//***********************************************************************************
// defined files
//***********************************************************************************

//***********************************************************************************
// private variables
//***********************************************************************************
int rxdata=0;
//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	 configures a LEUART peripheral
 *
 * @details
 *	Enables communication over LEUART and its startframe, sigframe, RX,TX
 *	and interrupts
 *
 * @param leuart
 *   LEUART module desired to be used
 *
 * @param leuart_settings
 * 	Settings desired on how to configure the LEAURT peripheral
 *
 ******************************************************************************/
void leuart_open(LEUART_TypeDef *leuart, LEUART_OPEN_STRUCT *leuart_settings){
	if(leuart == LEUART0){
			CMU_ClockEnable (cmuClock_LEUART0 , true);
		}
	uint32_t	StartFrame	    =		leuart->STARTFRAME;
	leuart->STARTFRAME			=		(~StartFrame);
	while(leuart->SYNCBUSY);
	EFM_ASSERT(leuart->STARTFRAME  &  ~StartFrame);
	leuart->STARTFRAME			=		StartFrame;

	// put the value
	LEUART_Init_TypeDef leuart_settings_2;
	leuart_settings_2.enable	=		leuart_settings->enable;
	leuart_settings_2.refFreq	=		leuart_settings->reffreq;
	leuart_settings_2.baudrate	=		leuart_settings->baudrate;
	leuart_settings_2.databits	=		leuart_settings->databits;
	leuart_settings_2.parity	=		leuart_settings->parity;
	leuart_settings_2.stopbits	=		leuart_settings->stopbits;

	LEUART_Init(leuart,&leuart_settings_2);
	while(leuart->SYNCBUSY);
	// enable frame detection
	leuart->ROUTEPEN			=		LEUART_ROUTEPEN_TXPEN | LEUART_ROUTEPEN_RXPEN;
	leuart->ROUTELOC0			=		leuart_settings->LEUART_RX |
										(leuart_settings->LEUART_TX << _LEUART_ROUTELOC0_TXLOC_SHIFT);
	// clear register
	leuart->CMD				    |=		(LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX);
	//enable register
	leuart->CMD				    |=		(LEUART_CMD_TXEN | LEUART_CMD_RXEN);
	//   leuart->IEN				   |=		(LEUART_IEN_STARTF | LEUART_IEN_SIGF);
	LEUART_Enable(leuart,leuart_settings->enable);
	while(leuart->SYNCBUSY);

	EFM_ASSERT(leuart->STATUS	&  (LEUART_STATUS_RXENS | LEUART_STATUS_TXENS));
	NVIC_EnableIRQ(LEUART0_IRQn);
}
/***************************************************************************//**
 * @brief
 *	 LEUART Interrupts
 *
 * @details
 *	Sends data when available, catches completed transmissions and resets private variables
 *	checking the buffer  timer to rx and tx data value to generate interrupt
 *  taking interrupt to set start and signal frame that can be inputed from ble terminal
 *
 * 	Begin receiving incoming frame,Since we have received the start frame, enable the RXDATAV
 * 	if we have already received a frame, and we receive another StartFrame then reset
 *
 * 	Stop Receiving bytes and parse the command that was received,
 * 	Set rxdata to 0 when sigframe is detected then Re-enable RXBLOCKEN
 * 	and start reading the temperature
 *
 *  Check if startframe has been repeated before sigframe received,
 *  When sigframe has been received, RX transmission is stopped
 *  rx receive data 1 byte at a time and store in the show_value array
 ******************************************************************************/
void LEUART0_IRQHandler(void){
	__disable_irq();
	uint32_t int_flag = LEUART0->IF & LEUART0->IEN;
	LEUART0->IFC		 |= int_flag;

	if(int_flag & LEUART_IF_TXBL){
		LEUART0->IEN         &=  ~LEUART_IEN_TXBL;
		leuart_IO.To_TX		  =	  true;
		if(leuart_IO.count_TX <= leuart_IO.n_TX){
			leuart_app_transmit_byte(LEUART0,leuart_IO.ReciveBuffer[leuart_IO.count_TX]);
			leuart_IO.count_TX++;
			LEUART0->IEN	|= 	LEUART_IEN_TXBL;
		}
		if(leuart_IO.count_TX == leuart_IO.n_TX){
			LEUART0->IEN	|=  LEUART_IEN_TXC;
			LEUART0->IEN	&= ~LEUART_IEN_TXBL; // DISABLE TXBL INTERRUPT
		}
	}

	if(int_flag & LEUART_IF_TXC){
		if(leuart_IO.count_TX == leuart_IO.n_TX){
			LEUART0->IEN	&=	 ~LEUART_IF_TXC;
			add_scheduled_event(LEUART0_TX_DONE_EVT);
			sleep_unblock_mode(LEUART_EM_BLOCK);
			leuart_IO.count_TX		=	0;
			leuart_IO.n_TX			=	0;
		}
	}
	if(int_flag & LEUART_IF_STARTF){
		if(leuart_IO.state){
				rxdata=0;
		}
		leuart_IO.state	=	true;
		LEUART0->IEN	|=	LEUART_IEN_RXDATAV;

	}
	if(int_flag	&	LEUART_IF_RXDATAV){
		show_value[rxdata]	=	leuart_app_receive_char_byte(LEUART0);
		rxdata++;
		LEUART0->IEN	|=		LEUART_IEN_RXDATAV;
	}
	if( int_flag & LEUART_IF_SIGF){
		//add_scheduled_event(LEUART0_SIG);
		LEUART0->CMD	 =		LEUART_CMD_RXBLOCKEN;
		LEUART0->IEN	&=		~LEUART_IEN_RXDATAV;
		if(leuart_IO.state){
			 reg_sig = BLE_return(show_value);
		}
		leuart_IO.state = false;
		leuart_IO.count_TX		=	0;
		leuart_IO.n_TX			=	0;

	}

	__enable_irq();
}
/***************************************************************************//**
 * @details
 * uint32_tleuart_status(LEUART_TypeDef*leuart);
 * Returns the value of the LEUART status register
 * voidleuart_cmd_write(LEUART_TypeDef*leuart, uint32_tcmd_update);
 * Writes the value of cmd_update to the LEUART CMD
 * registeruint32_tleuart_ien_test_get(LEUART_TypeDef*leuart);
 * Returns the value in the LEUART IEN register
 * voidleuart_ien_reset(LEUART_TypeDef*leuart, uint32_tien_reg);
 * Clears all pending interrupts in the LEUART IF register
 * Write the ien_reg into the LEUART IEN register.
 * It is not an OR operation with the IEN register.
 * voidleuart_app_transmit_byte(LEUART_TypeDef*leuart, uint8_tdata_out);
 * Sends a single byte to the LEUART TXDATA buffer
 * uint8_tleuart_app_receive_byte(LEUART_TypeDef*leuart);
 * Polls until a byte is available in the LEUART RXDATA buffer
******************************************************************************/

/***************************************************************************//**
 * @brief
 *	 LEUART define for transmitting byte
 *
 * @details
 *	Blocks a sleep mode then configuring private variables to  begins transmission
 *
 * @param leuart
 *   LEUART module
 *
 * @param string
 *   String  to be sent over LEUART
 *
 * @param string_len
 *   length of string
 *
 ******************************************************************************/
void leuart_start(LEUART_TypeDef *leuart, char *string, uint32_t string_len){
		sleep_block_mode(LEUART_EM_BLOCK);
		strcpy(leuart_IO.ReciveBuffer,string);

		leuart_IO.n_TX				=	string_len;
		leuart_IO.count_TX			= 	0;
		leuart_IO.To_TX				=	true;
		leuart_IO.state				=	true;
		leuart_transmit_byte (leuart);
}
/***************************************************************************//**
 * @brief
 *	Checks to see if the LEUART module is busy SYNCBUSY
 *
 * @details
 *	Checks to see if the LEUART module is busy SYNCBUSY
 *
 * @param leuart
 *   LEUART module
 *
 *  @return
 *   true if the leuart -> SYNCBUSY
 *
 ******************************************************************************/
bool leuart_tx_busy(LEUART_TypeDef *leuart){
	return leuart->SYNCBUSY;
}

/***************************************************************************//**
 * @brief
 * 	Disables LEUART's RX
 *
 * @details
 *	Sends command to disable the RX
 *
 * @param leuart
 *   LEUART module
 *
 ******************************************************************************/
void leuart_rx_disable(LEUART_TypeDef *leuart) {
	leuart->CMD		   &=   ~(LEUART_CMD_RXEN);
}
/***************************************************************************//**
 * @brief
 * 	Transmitting LEUART's TX
 *
 * @details
 *	Sends command to enable tx
 *
 * @param leuart
 *   LEUART module
 *
 ******************************************************************************/

void leuart_transmit_byte(LEUART_TypeDef *leuart){
	leuart->IEN		   |=	 (LEUART_IEN_TXBL);
}

/***************************************************************************//**
 * @brief
 *	Returns the status  of LEUART
 *
 * @details
 *	gives the status  of LEUART
 *
 * @param leuart
 *   LEUART module
 *
 *  @return
 *  leuart status
 *
 ******************************************************************************/
uint32_t leuart_status(LEUART_TypeDef *leuart){
	return leuart->STATUS;
}

/***************************************************************************//**
 * @brief
 *	send cmd_update to LEUARTx->CMD register
 *
 * @details
 *	Writes a cmd_update to LEUART and check SYNCBUSY
 *
 * @param leuart
 *  LEUART module
 *
 * @param cmd_update
 * 	Command desired to be written into CMD
 *
 ******************************************************************************/
void leuart_cmd_write(LEUART_TypeDef *leuart, uint32_t cmd_update){
	while(leuart->SYNCBUSY);
	leuart->CMD         =      cmd_update;
	while(leuart->SYNCBUSY);
}

/***************************************************************************//**
 * @brief
 *	gives IEN register of LEUART
 *
 * @details
 *	Returns the IEN register of LEUART
 *
 * @param leuart
 *   LEUART module desired
 *
 *  @return
 *   leuart IEN register
 *
 ******************************************************************************/
uint32_t leuart_ien_test_get(LEUART_TypeDef *leuart){
	uint32_t	get_ien	=	   leuart->IEN;
	leuart->IEN			=	   0;
	return      get_ien;

}

/***************************************************************************//**
 * @brief
 *	Resets the IEN register and reconfigures leuart
 *
 * @details
 *	Resets the IEN register of leaurt and reconfigure leuart
 *
 * @param leuart
 *   LEUART module
 *
 * @param ien_reg
 * 	The interrupts  to be enabled
 *
 ******************************************************************************/
void leuart_ien_reset(LEUART_TypeDef *leuart, uint32_t ien_reg){
	LEUART_IntClear(leuart,_LEUART_IFC_MASK);
	leuart->IEN			=	   ien_reg;
}
/***************************************************************************//**
 * @brief
 *  app byte configuration
 *
 * @details
 * transmitting  byte over LEUART
 *
 * @param leuart
 *   LEUART module
 *
 * @param data_out
 * 	Byte to be transmitted
 *
 ******************************************************************************/

void leuart_app_transmit_byte(LEUART_TypeDef *leuart, uint8_t data_out){
	while(!(leuart->STATUS & LEUART_STATUS_TXBL)){
		enter_sleep();
	}
	while(leuart->SYNCBUSY & LEUART_SYNCBUSY_TXDATA);
	leuart->TXDATA		=     (uint32_t) data_out;

}

/***************************************************************************//**
 * @brief
 *	 byte to be received  using leuart in leuart
 *
 * @details
 *	Waits for a byte to be received over leuart
 *
 * @param leuart
 *   LEUART module
 *
 * @return
 * Byte received in uint8_t
 *
 ******************************************************************************/
uint8_t leuart_app_receive_byte (LEUART_TypeDef *leuart){
	while(!(leuart->STATUS & LEUART_STATUS_RXDATAV));
	uint8_t LeuartData				= (uint8_t)   leuart->RXDATA;
	return  LeuartData;
}
/***************************************************************************//**
 * @brief
 *	 byte to be received  using leuart in ascii char
 *
 * @details
 *	Waits for a byte to be received over leuart which is ascii cahr
 *
 * @param leuart
 *   LEUART module
 *
 * @return
 * Byte received in char
 *
 ******************************************************************************/
char leuart_app_receive_char_byte(LEUART_TypeDef *leuart){
	while(leuart->SYNCBUSY);
	char LeuartData				= (char)   leuart->RXDATA;
	return  LeuartData;
}
