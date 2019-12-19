
/**
* @file BLE_communication.c
* @author Shaswat Koirala
* @date December 12th, 2019
* @brief setting up BLE communication  System
* @details
* read BLE module command and operate the task
* setup leuart startframe and then signal frame to communicate between app to device
* @note
* interact with BLE.c file to communicate with BLE terminal
*/
//***********************************************************************************
// Include files
//***********************************************************************************

//**  Standard Libraries

//**  Silicon Labs include files
#include "em_cmu.h"
#include "em_assert.h"
#include "SI7021.h"
#include <stdio.h>
#include "app.h"
//**  User/develop include files
#include "BLE_communication.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************

 char show_value[8];

//***********************************************************************************
// functions
//***********************************************************************************
// check if the BLE terminal command is properly communicating with leuart

 /***************************************************************************//**
  * @ brief
  *  BLE communication test for proper assign of start and sig frame
  *
  * @details
  * setting up proper communication between SI7021, leuart and bluetooth ble
  * by sendind the pre assigned start and sig frame. checking the tx and rx
  * value of leuart. setting up assert to check if those frames were properly
  * assigned while SYNCBUSY and then reseting TX bytes. Saving data over to
  * RX bytes. Checking following assert Not startframe, Startframe and not sigframe ,and Startframe and sigframe
  *
  *@param leuart
  *  LEUART module
  ******************************************************************************/
void BLE_communication_test(LEUART_TypeDef * leuart){
//	bool data_recive;
// assign values to leuart, enabaling startframe and blocking RXdata
	uint8_t		LEUART_STARTFRAME  =         (uint8_t)BLE_COMM_STARTFRAME;
	uint8_t		LEUART_SIGFRAME    =         (uint8_t)BLE_COMM_SIGFRAME;
	leuart->CTRL 			      |=		 LEUART_CTRL_LOOPBK;
	leuart->STARTFRAME 			   = 		 LEUART_STARTFRAME;
	leuart->SIGFRAME			   =         LEUART_SIGFRAME;
	leuart->CTRL				  |= 		 LEUART_CTRL_SFUBRX;
	leuart->CMD					  |= 		 LEUART_CMD_RXBLOCKEN;
	uint8_t stackframe		   	   = 	 	 0;
	// check to see if data gets loaded wait until something is loaded on rxdata
	leuart_app_transmit_byte(leuart,~LEUART_STARTFRAME);
	while(!(leuart->IF & LEUART_IF_TXC));
	//check to see if the startframe is not detected, and check data in rxdata is empty
	EFM_ASSERT(!(leuart->IF & LEUART_IF_STARTF));
	EFM_ASSERT(leuart->RXDATA == 0x00);
	// RESET TXC BIT
	leuart->IFC	                  |= 		 LEUART_IFC_TXC;
	// SYNCBUSY CHECK
	while(leuart->SYNCBUSY);

     // check to see if data gets loaded when startframe that is send
	leuart_app_transmit_byte(leuart, LEUART_STARTFRAME);
	while(!(leuart->IF & LEUART_IF_TXC));
	while(!(leuart->IF & LEUART_IF_STARTF));

	EFM_ASSERT(leuart->IF & LEUART_IF_STARTF);
	EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXDATAV);

	stackframe	     			   = 		  leuart->RXDATA;
	EFM_ASSERT(stackframe == LEUART_STARTFRAME);
	leuart->IFC					  |=         LEUART_IFC_STARTF;
	// RESET TXC BIT
	leuart->IFC	                  |=	   	  LEUART_IFC_TXC;
	// SYNCBUSY CHECK
	while(leuart->SYNCBUSY);

	//sending wrong value to establish rxdata
	// transmitting sig vlaue
	leuart_app_transmit_byte(leuart,(uint8_t)~LEUART_SIGFRAME);
	while(!(leuart->IF & LEUART_IF_TXC));
	EFM_ASSERT(!(leuart->IF & LEUART_IF_SIGF));
	stackframe                     = 		  leuart->RXDATA;

	leuart->CMD					  |= 		  LEUART_CMD_RXBLOCKEN;
	leuart->IFC 				  |= 		  (LEUART_IFC_STARTF | LEUART_IFC_SIGF);
	// RESET TXC BIT
	leuart->IFC	                  |= 	       LEUART_IFC_TXC;
	// SYNCBUSY CHECK
	while(leuart->SYNCBUSY);
   // sending correct value
	leuart_app_transmit_byte(leuart, LEUART_STARTFRAME);
	while(!(leuart->IF & LEUART_IF_TXC));
	while(!(leuart->IF & LEUART_IF_STARTF));
	EFM_ASSERT(leuart->IF & LEUART_IF_STARTF);
	EFM_ASSERT(leuart->STATUS & LEUART_STATUS_RXDATAV);
	stackframe       			   = 		   leuart->RXDATA;
	EFM_ASSERT(stackframe == LEUART_STARTFRAME);

	// SYNCBUSY CHECK
	while(leuart->SYNCBUSY);
    //send signal frame
	leuart_app_transmit_byte(leuart,(uint8_t)LEUART_SIGFRAME);
	while(!(leuart->IF & LEUART_IF_TXC));
	while(!(leuart->IF & LEUART_IF_SIGF));
	// SYNCBUSY CHECK
	while(leuart->SYNCBUSY)
		//clearing everything
	leuart->IFC 				  |= 		  (LEUART_IFC_STARTF | LEUART_IFC_SIGF);
	leuart->CMD 				  |= 		  LEUART_CMD_RXBLOCKEN;
	leuart->CTRL 				  &= 		  ~LEUART_CTRL_LOOPBK;
	// RESET TXC BIT
	leuart->IFC	           		  |= 	  	  LEUART_IFC_TXC;
	 ble_write("\n Communication successful \n");

}
/***************************************************************************//**
 * @details
 * low energy mode enable and disable to let
 * kit to be in sleep while not being used
 ******************************************************************************/


/***************************************************************************//**
  *  @ brief
  *  enable from sleep mode
  *
  * @details
  * Blocking sleep mode and then Enabling the SCL and SDA
  * lines to allow I2C communication and lastly
  * Resetting the I2C state machines on master and Si7021
  ******************************************************************************/
void enable_leuart_status(void){
	sleep_block_mode(EM2);
    GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeWiredAnd, false);
    GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeWiredAnd, false);
    i2c_bus_reset(Si7021_I2C,i2cIO);
}
/***************************************************************************//**
  *  @ brief
  *  disable from sleep mode
  *
  * @details
  * Disabling the SCL and SDA lines, then deserting the sensor enable and then
  * Unblocking sleep mode
  ******************************************************************************/
void disable_leuart_status(void){
    GPIO_PinModeSet(SI7021_SCL_PORT, SI7021_SCL_PIN, gpioModeDisabled, false);
    GPIO_PinModeSet(SI7021_SDA_PORT, SI7021_SDA_PIN, gpioModeDisabled, false);
    si7021_disable();
    sleep_unblock_mode(EM2);
}
/***************************************************************************//**
 * @brief
 *  display the temperature in BLE terminal
 *
 * @details
 *  low energy mode, when asked to receive data the systems wakes up and
 *  after temp_display it goes back to sleep
 *  goes to while loop to check if value from BLE is either to show temp in
 *  C or F
 *
  *@param leuart
  *  LEUART module
 *
 ******************************************************************************/
void temp_display(LEUART_TypeDef * leuart){
	float temp_fahrenheit =si7021_measure_temp();
	float temp_celsius    =si7021_measure_temp_celsius();
	while(reg_sig!=0){
	if (reg_sig==1){
		temp_display_F(temp_fahrenheit);
	}
	else if(reg_sig==2){
		temp_display_C(temp_celsius);
	}
	}

}
/***************************************************************************//**
 * @brief
 *  Display temperature in Fahrenheit
 *
 * @details
 *	display temperature in Fahrenheit when 'f' is transmitted over to BLE terminal
 *	where float is converted to char to be displayed
 *
 *	param temp_fahrenheit
 *	 float temperature value in Fahrenheit
 ******************************************************************************/
void temp_display_F(float temp_fahrenheit){
	char	temp[30] = "\n Temperature in Fahrenheit =";
	int 	fr		 =  (int)(temp_fahrenheit*10);
	int     d		 =   fr%10;
	if(d !=0){
		fr			 =	 (int)temp_fahrenheit;
	}
	else{
		fr			 = 	  fr/10;
	}
	char temp1[2] 		 =     {0};
	char temp2[2]		 = 	   {0};
	int_to_string(fr,2,temp1);
	strcat(temp,temp1);
	if(d !=0){
		    int_to_string(d,1,temp2);
			strcat(temp,".");
			strcat(temp,temp2);
		}
	strcat(temp,"\n");
	ble_write(temp);
}
/***************************************************************************//**
 * @brief
 *  Display temperature in celsius
 *
 * @details
 *	display temperature in celsius when 'c' is transmitted over to BLE terminal
 *	where float is converted to char to be displayed
 *
 *	param temp_celsius
 *	 float temperature value in celsius
 ******************************************************************************/
void temp_display_C(float temp_celsius){
	char	temp[30] = "\n Temperature in celsius =";
				int 	cl		 =  (int)(temp_celsius*10);
				int     d		 =   cl%10;
				if(d !=0){
					cl			 =	 (int)temp_celsius;
					}
				else{
						cl			 = 	  cl/10;
					}
				char temp1[2] 		 =     {0};
				char temp2[2]		 = 	   {0};
				int_to_string(cl,2,temp1);
				strcat(temp,temp1);
				if(d !=0){
					    int_to_string(d,1,temp2);
						strcat(temp,".");
						strcat(temp,temp2);
						}
				strcat(temp,"\n");
				ble_write(temp);
}
/***************************************************************************//**
 * @brief
 *	Converts an integer to a string/char
 *
 * @details
 *	Converts an integer to a string/char
 *
 * @param n
 *   integer to be converted to a string
 *
 * @param s
 *   how many places is the integer being converted
 *
 * @param *c
 * 	 where to store the string
 *
 ******************************************************************************/
void int_to_string(int n,int s,char *c){
	int i=s;
	while(i>=0){
		if(i==s){
			c[i]='\0';
			i--;
		}
		else{
			c[i]=n%10;
			c[i]+=48;
			n=n/10;
			i--;
		}
	}

}/***************************************************************************//**
 * @brief
 * Define the state of required BLE command
 * @details
 * setting the value of show_val when input is received from BLE terminal
 * checking what user have sent and then returning int to display the
 * desired output
 *
 * @param *show_val
 *  char value that stores the input BLE command from terminal
 *
 * @return
 *  return int value as per command set in BLE terminal
 *  return 1 if ble terminal input is f ( for Fahrenheit output,
 *  	return 2 if input is c (for celsius output) else return 0
 ******************************************************************************/

int BLE_return(char *show_val){
if(show_val[0] != (char)LEUART0->STARTFRAME){
	return 0;
}
if(show_val[2] != (char)LEUART0->SIGFRAME){
	return 0;
}

 if(show_val[1] == 'c'){
	return 2;
}
 if(show_value[1] == 'f'){
	 return 1;
 }
return 0;
}





