/**
* @file i2c.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief Assigning I2C/ resetting and building interrupt
* @details
* Passed instruction determines the flow of I2C bus performing the called function
*/
//***********************************************************************************
// Include files
//***********************************************************************************
#include "i2c.h"
#include "Si7021.h"
#include "em_i2c.h"
#include "em_gpio.h"
#include "em_emu.h"

#include "scheduler.h"
#include "gpio.h"
#include "app.h"

static I2C_PAYLOAD_STRUCT Payload;


//***********************************************************************************
// functions
//***********************************************************************************

/***************************************************************************//**
 * @brief
 *	Initializing an I2C peripheral
 *
 * @details
 *	Initialize the I2C peripheral according to the structures passed after enabaling
 *	the clock and then handle inturrupt
 *
 *	@param i2c
 *  I2C port to for communication
 *
 *  @param i2cOpen
 *   I2C configuration for desired struct
 *
 *   @param i2cIO
 *   I2C configuration for desired Input output
 *    GPIO port and pin information for the SCL and SDA lines
 *
 * @note
 *   using I2C1 has some other configuration
 *
 ******************************************************************************/
void i2c_Open(I2C_TypeDef *i2c,I2C_OPEN_STRUCT i2cOpen,I2C_IO_STRUCT i2cIO){
	if(i2c == I2C0){
		CMU_ClockEnable (cmuClock_I2C0 , true);
	}
	else if (i2c == I2C1){
		CMU_ClockEnable (cmuClock_I2C1 , true);
	}

	if((i2c->IF & 0x01) ==0){
		i2c->IFS=0x01;
		EFM_ASSERT(i2c->IF & 0x01);
		i2c->IFC=0x01;
	}
	else {
		i2c->IFC=0x01;
		EFM_ASSERT(!(i2c->IF & 0x01));
}
		I2C_Init_TypeDef	i2cInit	=	I2C_INIT_DEFAULT;
		i2cInit.enable	=	i2cOpen.enable;
		i2cInit.master	=	i2cOpen.master;
		i2cInit.refFreq	=	i2cOpen.refFreq;
		i2cInit.freq	=	i2cOpen.freq;
		i2cInit.clhr	=	i2cOpen.Clhr;

		I2C_Init(i2c,&i2cInit);


		i2c->ROUTELOC0 &= ~(_I2C_ROUTELOC0_SDALOC_MASK | _I2C_ROUTELOC0_SCLLOC_MASK);
		i2c->ROUTELOC0  |=  i2cOpen.SDALOC;
		i2c->ROUTELOC0  |=  (i2cOpen.SCLLOC << _I2C_ROUTELOC0_SCLLOC_SHIFT);


		//PIN
		i2c->ROUTEPEN  =  I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
		i2c_bus_reset(i2c,i2cIO);

		i2c->IEN	=	0;
		i2c->IEN 	|= (I2C_IEN_RXDATAV | I2C_IEN_ACK | I2C_IEN_NACK |I2C_IEN_MSTOP);



		//enabling the interrupt at CPU level
		if(i2c==I2C0){
		NVIC_EnableIRQ(I2C0_IRQn);
		}
		else if (i2c==I2C1){
			NVIC_EnableIRQ(I2C1_IRQn);
		}
		I2C_IntClear(i2c,_I2C_IFC_MASK);
		I2C_Enable(i2c,true);

}
/***************************************************************************//**
 * @brief
 *  Resets the I2C bus
 *
 * @details
 *  Sending toggles on the SDA  to clear the i2c bus or reseting it
 *
 * @param i2c
 * 	I2C port to start communication
 *
 * @param i2cIO
 *  I2C configuration for desired Input output
 *  GPIO port and pin information for the SCL and SDA lines
 ******************************************************************************/
void i2c_bus_reset(I2C_TypeDef *i2c,I2C_IO_STRUCT i2cIO){

	EFM_ASSERT(GPIO_PinInGet(i2cIO.SCL_PORT,i2cIO.SCL_PIN));
	EFM_ASSERT(GPIO_PinInGet(i2cIO.SDA_PORT,i2cIO.SDA_PIN));
	//lower power mode
	int i;
	for(i=0; i <9;i++){
		GPIO_PinOutClear(i2cIO.SDA_PORT,i2cIO.SDA_PIN);
		GPIO_PinOutSet(i2cIO.SDA_PORT,i2cIO.SDA_PIN);
	}
	i2c->CMD	=	I2C_CMD_ABORT;

}

/***************************************************************************//**
 * @brief
 *  initializing the  I2C state machine with the given parameters
 *
 * @details
 *  initialize the Payload structure  variable, starting the state machine,
 *  and  START bit with the slave address
 *
 * @param i2c
 * 	I2C port to start communicating on
 *
 * @param SlaveDevice
 * 	Device that will be in communication
 *
 * @param RegAdr
 * 	 register on the slave device that is to be read/written to
 *
 * @param RW
 * 	used for communication reading or writing data
 *
 * @param *buffer
 * 	to store received data
 *
 ******************************************************************************/
void i2c_start(I2C_TypeDef *i2c,uint8_t SlaveDevice, uint8_t RegAdr, I2C_Read_Write RW, uint16_t *buffer){
			EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK)==I2C_STATE_STATE_IDLE);
			sleep_block_mode(I2C_EM_BLOCK);
// after Energry mode 2 store the value to pass it to si7021.c
			Payload.i2c				=	i2c;
			Payload.RW 				=	RW;
			Payload.DeviceAdr		=	SlaveDevice;
			Payload.RegAdr			=	RegAdr;
			Payload.SystemState		=	Start;
			Payload.buffer			=	buffer;

			Payload.i2c->CMD		=	I2C_CMD_START;
			Payload.i2c->TXDATA		=	(Payload.DeviceAdr <<1)	|	i2c_write;

}
/***************************************************************************//**
 * @brief
 *  It is an ACK interrupt occurs function
 *
 * @details
 * 	If an ACK has received progress from the state machine to
 * 	store current state into Payload
 *
 ******************************************************************************/
void I2C_ACK(void){
		if(Payload.RW == I2C_Read) {
			switch(Payload.SystemState) {
				case(Start): {
					Payload.SystemState = RegSend;		//state to go in RegSend Case
					Payload.i2c->TXDATA = Payload.RegAdr;
					break;
				}
				case(RegSend): {
					Payload.SystemState = Re_Start; //state to go in Re_Start Case
					Payload.i2c->CMD	= I2C_CMD_START;
					Payload.i2c->TXDATA	= (Payload.DeviceAdr <<1)| i2c_read;
					break;
				}
				case(Re_Start): {
					Payload.SystemState = SlaveData;  //state to go in SlaveDate Case

					break;
				}
				case(Stop):
						break;
				case(SlaveData):
						break;
			}
		}

}

/***************************************************************************//**
 * @brief
 *  when NACK interrupt occurs
 *
 * @details
 * 	If an ACK has received progress from the state machine to
 * 	store current state into Payload
 *
 * @note
 * 	 SI7021 will NACK until the measurement is complete re-occuring in reset
 *
 ******************************************************************************/
void I2C_NACK(void){
	if(Payload.RW == I2C_Read) {
		if(Payload.SystemState == Re_Start) {
				Payload.i2c->CMD	|=	I2C_CMD_START;
				Payload.i2c->TXDATA  = (Payload.DeviceAdr<<1)	|	i2c_read;

		}
		else {
			Payload.SystemState	  = 	Start;
			Payload.i2c->CMD	 |=	    I2C_CMD_START;
			Payload.i2c->TXDATA	  = 	(Payload.DeviceAdr <<1 ) |	i2c_write;
		}

	}

}
/***************************************************************************//**
 * @brief
 *  put bytes of received data into the Payload  buffer
 *
 * @details
 *  If data is received put it into the Payload buffer
 * after another byte is received  then send a NACK and stop after that
 * set a SI7021 done event
 ******************************************************************************/
void I2C_RXDATAV(void){
	static bool flag = false;
	if(Payload.SystemState == SlaveData) {
		if(flag == true) {
				flag				= false;
				*Payload.buffer 	|= I2C0->RXDATA;
				 Payload.i2c->CMD   = I2C_CMD_NACK;
				 Payload.i2c->CMD   = I2C_CMD_STOP;
				 add_scheduled_event(SI7021_TEMP_DONE);
		}
		else if(flag == false) {
			flag 					= true;
			*Payload.buffer 		= I2C0->RXDATA <<8;
			I2C0->CMD 				= I2C_CMD_ACK;
		}
	}
}
/***************************************************************************//**
 * @brief
 *  I2C0 Interrupt Handler
 *
 * @details
 *  Read the I2C0 bus interrupts, clear them, and call
 *  functions that can be used depending on what was received
 *
 ******************************************************************************/
void I2C0_IRQHandler(void){
	__disable_irq();
	 int int_flag = I2C0->IF;
	I2C_IntClear(I2C0,int_flag);
	if(int_flag & I2C_IF_ACK){
		I2C_ACK();
	}
	if(int_flag & I2C_IF_NACK){
			I2C_NACK();
		}
	if(int_flag & I2C_IF_RXDATAV){
			I2C_RXDATAV();
		}
	if(int_flag & I2C_IF_MSTOP){
		sleep_unblock_mode(I2C_EM_BLOCK);
	}
	__enable_irq();
}
/***************************************************************************//**
 * @brief
 *  I2C1 Interrupt Handler
 *
 * @details
 *  Read the I2C1 bus interrupts, clear them, and call
 *  functions that can be used depending on what was received
 *
 ******************************************************************************/
void I2C1_IRQHandler(void){
	__disable_irq();
	 int int_flag = I2C1->IF;
	I2C_IntClear(I2C1,int_flag);
	if(int_flag & I2C_IF_ACK){
		I2C_ACK();
	}
	if(int_flag & I2C_IF_NACK){
			I2C_NACK();
		}
	if(int_flag & I2C_IF_RXDATAV){
			I2C_RXDATAV();
		}
	if(int_flag & I2C_IF_MSTOP){
		sleep_unblock_mode(I2C_EM_BLOCK);
	}
	__enable_irq();
}

