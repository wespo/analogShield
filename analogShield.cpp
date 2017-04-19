/************************************************************************/
/*																		*/
/*	analogShield.h	--	Library for Analog Shield on DSP Shield 		*/
/*     Version - 1.0                                                    */
/*																		*/
/************************************************************************/
/*	Author: 	William J. Esposito										*/
/*	Copyright 2015, William Esposito.									*/
/************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*	This module contains the implementation of the object class that	*/
/*	forms a DSP Shield interface to the Analog Shield functions of  	*/
/*  the Texas Instrument Analog Shield.									*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	05/24/2015(WilliamE): Branched										*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <analogShield.h>
analogShield analog;
	
/* ------------------------------------------------------------ */
/*        analogShield::analogShield
**
**        Synopsis:
**			none
**        Parameters:
**			none
**        Errors:
**			none
**
**        Description:
**			Default constructor that sets the tristate buffers
**			for the ADC and DAC control shieldPins 
**
**
*/	
    analogShield::analogShield() {	
		//Initialize Private Variables
		shieldMode = 0;

		//default shieldPins
		shieldPins.adccsPin = adccs;
		shieldPins.adcbusyPin = adcbusy;
		shieldPins.daccsPin = daccs;
		shieldPins.dacldPin = dacld;

		SPI.begin();
	}

/* ------------------------------------------------------------ */
/*        analogShield::begin
**
**        Synopsis:
**				begin();
**        
**		  Parameters:
**				none
**
**        Return Values:
**              none 
**
**        Errors:
**			
**        Description:
**			Initializes the SPI interface on the microcontroller 
**
**
*/
	
	void analogShield::begin(){

		analog.configureShieldPins();
	    //gpioPinMode(DSP_GPIO13, OUTPUT);
		
		//digitalWrite(ARD_SPI_EN, HIGH);
		// digitalWrite(adccs, HIGH);
		// digitalWrite(daccs,HIGH);    
		// digitalWrite(dacld,LOW);

		//pinMode(ARD_SPI_EN, OUTPUT);
		// pinMode(adccs, OUTPUT);
		// pinMode(adcbusy, INPUT);
		// pinMode(daccs, INPUT);
		// pinMode(dacld, OUTPUT);

		// gpioPinMode(DSP_GPIO13, OUTPUT); //DACCS
		// gpioWrite(DSP_GPIO13, HIGH);

		// gpioPinMode(DSP_GPIO14, OUTPUT); //ADCCS
		// gpioPinMode(DSP_GPIO12, INPUT);  //ADCBUSY
		

 		/* Initialize DSP SPI peripheral */
 		SPI.setLoopBackMode(0);
	    // SPI.setBitOrder(MSBFIRST);
     //  	SPI.setDataMode(SPI_MODE2);
     //  	SPI.setClockDivider(SPI_CLOCK_DIV128);
      	pinMode(adccs, INPUT);
	}

	
/* ------------------------------------------------------------ */
/*        analogShield::end
**
**        Synopsis:
**				end();
**        
**		  Parameters:
**				none
**
**        Return Values:
**              none 
**
**        Errors:
**			
**        Description:
**			Closes the SPI communication interface 
**
**
*/	
	void analogShield::end(){
			SPI.end();
	}

/* ------------------------------------------------------------ */
/*				Helper Functions                                */
/* ------------------------------------------------------------ */
	
/* ------------------------------------------------------------ */
/*        analogShield::setChannelAndModeByte
**
**        Synopsis:
**				setChannelAndModeByte(channel, mode);
**        
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				bool mode - boolean to activate differential mode
**
**        Return Values:
**              none 
**
**        Errors:
**				none
**        Description:
**				Helper function that sets the channel and mode 
**				control byte and sent it over SPI 
**
**
*/	
	void analogShield::setChannelAndModeByte(int channel, bool mode){		
		//control byte
		//S - sentinel, always 1
		//A2 - channel select
		//A1 - channel select
		//A0 - channel select
		//- N/C
		//Single / Diff pair
		//PD1 - power down mode
		//PD0 - power down mode
		unsigned int control = 0x82; //default to channel 1 '001': B10000010
		
		//channel mask
		if(channel == 3){
			control = control | 0xE0; //B11100000
		}
		
		else if(channel == 2){
			control = control | 0xA0; //B10100000
		}
		
		else if(channel == 1){
			control = control | 0xD0; //B11010000
		}
		
		else if(channel == 0){
			control = control | 0x10; //B00010000
		}
		
		//differential mode active
		if(mode){
			control = control & 0xFB; //B11111011
		}
		
		else{
			control = control | 0x04; //B00000100
		}
		SPI.write(&control, 1);

		return;
	}


/* ------------------------------------------------------------ */
/*				ADC Functions                                   */
/* ------------------------------------------------------------ */
	
/* ------------------------------------------------------------ */
/*        analogShield::read
**
**        Synopsis:
**				read(channel, mode);
**        
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				bool mode - boolean to activate differential mode
**
**        Return Values:
**              unsigned int - Raw data from 0-65535 corresponding 
**				to the voltage presented to the ADC 
**
**        Errors:
**				none
**
**        Description:
**				Reads a raw data value from the ADC using the 
**				SPI interface
**
**
*/		
	unsigned int analogShield::read(int channel, bool mode) {
		int result = signedRead(channel, mode);
		
		//make into an unsigned int for compatibility with the DAC used on the analog shield.
		return result+32767;

	}

	
/* ------------------------------------------------------------ */
/*        analogShield::signedRead
**
**        Synopsis:
**				signedRead(channel, mode);
**        
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				bool mode - boolean to activate differential mode
**
**        Return Values:
**              int - Signed data from -32768 - 32767 corresponding 
**				to the voltage presented to the ADC 
**
**        Errors:
**				none
**
**        Description:
**				Reads a signed data value from the ADC using the 
**				SPI interface
**
**
*/		
	// int analogShield::signedRead(int channel, bool mode) {
	// 	// initialize SPI:
	// 	configureShieldMode(2);
		
	// 	// take the SS shieldPin low to select the chip, transfer the command, then bring the bit back high:
	// 	shieldPinReadWrite(adccs, LOW);

	// 	setChannelAndModeByte(channel, mode);
		
	// 	delayMicroseconds(10);

	// 	//shieldPinReadWrite(adccs, HIGH);
		
	// 	//delayMicroseconds(5);

	// 	//wait for busy signal to fall. If it lasts a while, try resending.
	// 	unsigned long count = 0;
	// 	//xfOff();
	// 	// while(shieldPinReadWrite(adcbusy) == 1){
	// 	// 	if (count > 1000)
	// 	// 	{
	// 	// 		xfOn();
	// 	// 		//setChannelAndModeByte(channel, mode);
	// 	// 		return 0;
	// 	// 		// shieldPinReadWrite(adccs, HIGH);
	// 	// 		// return 0;
	// 	// 	}
	// 	// 	delayMicroseconds(1);
	// 	// 	count++;
	// 	// } //wait for shieldPin 3 to == 0

	// 	//Result ready. Read it in
	// 	//shieldPinReadWrite(adccs, LOW);

	// 	//collect data
	// 	unsigned int readValue[2] = {};
	// 	SPI.read(readValue, 2);
	// 	delayMicroseconds(3);
	// 	//release chip select
	// 	shieldPinReadWrite(adccs, HIGH);

	// 	//compile the result into a 16 bit integer.
	// 	//int result = readValue[1];
	// 	int result = ((readValue[0] << 8) & 0xFF00) | (readValue[1] & 0x00FF);
	// 	return result;
	// }
	int analogShield::signedRead(int channel, bool mode) {

		//delayMicroseconds(100);
		// initialize SPI:
		configureShieldMode(2);
		
		// take the SS shieldPin low to select the chip, transfer the command, then bring the bit back high:
		shieldPinReadWrite(adccs, LOW);

		setChannelAndModeByte(channel, mode);
		
		//shieldPinReadWrite(adccs, HIGH);
		
		delayMicroseconds(1);

		//wait for busy signal to fall. If it lasts a while, try resending.
		// unsigned long count = 0;
		// while(shieldPinReadWrite(adcbusy) == 1){
		// 	if (count > 1000)
		// 	{
		// 		xfOn();
		// 		setChannelAndModeByte(channel, mode);
		// 		delayMicroseconds(15);
		// 		break;
		// 		// shieldPinReadWrite(adccs, HIGH);
		// 		// return 0;
		// 	}
		// 	count++;
		// } //wait for shieldPin 3 to == 0
		// if(count < 1000)
		// {
		// 	xfOff();
		// }
		// if(count < 1000) //if read didn't time out.
		// {
		// 	count = 0;
		// 	while(shieldPinReadWrite(adcbusy) == 1){
		// 		if (count > 100)
		// 		{
		// 			shieldPinReadWrite(adccs, HIGH);
		// 			//return 0;

		// 			xfOn();
		// 			break;
		// 		}
		// 		count++;
		// 	} //wait for shieldPin 3 to == 0
		// }
		// else
		// {
		// 	xfOff();
		// }

		//Result ready. Read it in
		//shieldPinReadWrite(adccs, LOW);

		//collect data
		unsigned int readValue[3] = {};
		SPI.read(readValue, 3);
		// delayMicroseconds(10);
		// SPI.read(readValue+1, 1);
		//release chip select
		shieldPinReadWrite(adccs, HIGH);

		SPI.setClockDivider(SPI_CLOCK_DIV2);
		unsigned int message[3] = {0};
		SPI.write(message, 3);
		//compile the result into a 16 bit integer.
		#ifdef BOARD_DSPSHIELD_V1 //for C5535, a hack.
		int result = (readValue[0] << 14) + (readValue[1] << 6) + (readValue[1] >> 2);
		#else 
		int result = (readValue[0] << 16) + (readValue[1]<< 8) + (readValue[2] >> 0);
		#endif
		
		return result;
	}

/* ------------------------------------------------------------ */
/*				DAC Functions                                   */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*        analogShield::writeNoUpdate
**
**        Synopsis:
**				writeNoUpdate(channel, value);
**        
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				unsigned int value - raw data value to write to 
**					the DAC
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes a raw data value to the selected DAC data	
**					buffer but does not update the DAC output
**
**
*/	
	void analogShield::writeNoUpdate(int channel, unsigned int value){
		// take the SS shieldPin low to select the chip:
		shieldPinReadWrite(daccs, LOW);
		
		//  send in the address and value via SPI:
		unsigned int call = 0x00;//00
		if(channel == 1)
			call = 0x02;
		
		else if(channel == 2)
			call = 0x04;
		
		else if(channel == 3)
			call = 0x06;
		

		unsigned int message[3] = { call, value >> 8, value & 0x00FF};
  		SPI.write(message, 3);
		shieldPinReadWrite(daccs, HIGH);
	}

/* ------------------------------------------------------------ */
/*        analogShield::writeAllUpdate
**
**        Synopsis:
**				writeAllUpdate(channel, value);
**        
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				unsigned int value - raw data value to write to 
**					the DAC
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes a raw data value to the selected DAC data	
**					buffer and updates DAC output
**
**
*/	
	void analogShield::writeAllUpdate(int channel, unsigned int value){
		// take the SS shieldPin low to select the chip:
		shieldPinReadWrite(daccs, LOW);
		
		//  send in the address and value via SPI:
		unsigned int call = 0x20;//20
		if(channel == 1)
			call = 0x22;
		
		else if(channel == 2)
			call = 0x24;
		
		else if(channel == 3)
			call = 0x26;
		
		unsigned int message[3] = { call, value >> 8, value & 0x00FF};
  		SPI.write(message, 3);
		
		// take the SS shieldPin high to de-select the chip:
		shieldPinReadWrite(daccs, HIGH);

	}

/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(channel, value);
**        
**		  Parameters:
**				int channel - The channel being set from 0 to 3
**				unsigned int value - raw data value to write to 
**					the DAC
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes a raw data value to the selected DAC data	
**					buffer and updates DAC output
**
**
*/	
	void analogShield::write(int channel, unsigned int value) {
		configureShieldMode(1);
	  
		// take the SS shieldPin low to select the chip:
		shieldPinReadWrite(daccs, LOW);

		//  send in the address and value via SPI:
		unsigned int call = 0x10;
		if(channel == 1)
		{
			call = 0x12;
		}
		else if(channel == 2)
		{
			call = 0x14;
		}
		else if(channel == 3)
		{
			call = 0x16;
		}

		unsigned int message[3] = { call, value >> 8, value & 0x00FF};
  		SPI.write(message, 3);

		// take the SS shieldPin high to de-select the chip:
		shieldPinReadWrite(daccs, HIGH);
	}

/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(value0, value1, simul);
**        
**		  Parameters:
**				unsigned int value0 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				unsigned int value1 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				bool simul - used to synchronize the DAC to update 
**					both channels at once
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes raw data values to the DAC.  
**					value0 => DAC channel 0
**					value1 => DAC channel 1 
**
*/	
	
	void analogShield::write(unsigned int value0, unsigned int value1, bool simul){
		configureShieldMode(1);
		
		writeNoUpdate(0,value0);
		writeAllUpdate(1,value1);
	}

/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(value0, value1, value2, simul);
**        
**		  Parameters:
**				unsigned int value0 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				unsigned int value1 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				unsigned int value2 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				bool simul - used to synchronize the DAC to update
**					both channels at once
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes raw data values to the DAC.  
**					value0 => DAC channel 0
**					value1 => DAC channel 1 
**					value2 => DAC channel 2  
**
**
*/		
	void analogShield::write(unsigned int value0, unsigned int value1, unsigned int value2, bool simul){
		configureShieldMode(1);
		
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeAllUpdate(2,value2);
	}


/* ------------------------------------------------------------ */
/*        analogShield::write
**
**        Synopsis:
**				write(value0, value1, value2, value3, simul);
**        
**		  Parameters:
**				unsigned int value0 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				unsigned int value1 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				unsigned int value2 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				unsigned int value3 - Raw value from 0 to 65535 
**					corresponding to -5V - 5V 
**				bool simul - used to synchronize the DAC to update 
**					both channels at once
**
**        Return Values:
**              none
**
**        Errors:
**				none
**
**        Description:
**				Writes raw data values to the DAC.  
**					value0 => DAC channel 0
**					value1 => DAC channel 1 
**					value2 => DAC channel 2 
**					value3 => DAC channel 3  
**
**
*/		
	void analogShield::write(unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3, bool simul){
		configureShieldMode(1);
		
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeAllUpdate(2,value2);
		writeAllUpdate(3,value3);
	}


int analogShield::configureShieldMode(int mode)
{
	if(mode == 1) //for DAC
	{
		if(shieldMode != 1){
			// initialize SPI:
			SPI.setBitOrder(MSBFIRST);
			SPI.setDataMode(SPI_MODE2);
			SPI.setClockDivider(SPI_CLOCK_DIV8);
			SPI.setCSPin(1);
			shieldMode = 1;
			return 1; //changed mode
		}
		return 0; //no change
	}
	else if(mode == 2) //for ADC
	{
		if(shieldMode != 2){
			SPI.setBitOrder(MSBFIRST);
			SPI.setDataMode(SPI_MODE3); //3
			SPI.setClockDivider(SPI_CLOCK_DIV128); //128
			SPI.setCSPin(0);
			shieldMode = 2;
			return 1; //changed mode
		}
		return 0; //no change
	}
	else
	{
		return -1; //invalid mode
	}

}
// configure pin to 0 to go back to using expander
void analogShield::configureShieldPin(int shieldPin, int gpioPin)
{
	switch (shieldPin) {
		case adccs:
			shieldPins.adccsPin = gpioPin;
			break;
		case adcbusy:
			shieldPins.adcbusyPin = gpioPin;
			break;
		case daccs:
			shieldPins.daccsPin = gpioPin;
			break;
		case dacld:
			shieldPins.dacldPin = gpioPin;
			break;
	}
	setupPins();

}
void analogShield::configureShieldPins()
{
	
	//SPI BUS CONNECT
	pinMode(SPI_RX_SEL, OUTPUT);
    pinMode(ARD_SPI_EN, OUTPUT);
    digitalWrite(SPI_RX_SEL, LOW);
    digitalWrite(ARD_SPI_EN, HIGH);

	//identify shieldPins
	shieldPins.adccsPin = findPin(adccs);
	shieldPins.adcbusyPin = findPin(adcbusy);
	shieldPins.daccsPin = findPin(daccs);
	shieldPins.dacldPin = findPin(dacld);

	setupPins();
}

void analogShield::setupPins()
{
	//configure shieldPins
	if(shieldPins.adccsPin == 0) //using expander
	{
		digitalWrite(adccs, HIGH);
		pinMode(adccs, OUTPUT);
	}
	else
	{
		pinMode(adccs, INPUT);
		gpioPinMode(shieldPins.adccsPin, OUTPUT);
		gpioWrite(shieldPins.adccsPin, HIGH);
	}

	if(shieldPins.adcbusyPin == 0) //using expander
	{
		//digitalWrite(adcbusy, LOW);
		pinMode(adcbusy, INPUT);
	}
	else
	{
		pinMode(adcbusy, INPUT);
		gpioPinMode(shieldPins.adcbusyPin, INPUT);
	}

	if(shieldPins.daccsPin == 0) //using expander
	{
		digitalWrite(daccs, HIGH);
		pinMode(daccs, OUTPUT);
	}
	else
	{
		pinMode(daccs, INPUT);
		gpioPinMode(shieldPins.daccsPin, OUTPUT);
		gpioWrite(shieldPins.daccsPin, HIGH);
	}	

	if(shieldPins.dacldPin == 0) //using expander
	{
		digitalWrite(dacld, LOW);
		pinMode(dacld, OUTPUT);
	}
	else
	{
		pinMode(dacld, INPUT);
		gpioPinMode(shieldPins.dacldPin, OUTPUT);
		gpioWrite(shieldPins.dacldPin, LOW);
	}	
}

void analogShield::printConfig()
{
	Serial.println("Pins: ");
	Serial.print("ADCCS: ");
	Serial.println(shieldPins.adccsPin);
	Serial.print("ADCBUSY: ");
	Serial.println(shieldPins.adcbusyPin);
	Serial.print("DACCS: ");
	Serial.println(shieldPins.daccsPin);
	Serial.print("DACLD: ");
	Serial.println(shieldPins.dacldPin);
}

int analogShield::findPin(int expanderPin)
{
	for(int gpioPin = 12; gpioPin < 16; gpioPin++) //check GPIO PINS 12-16
	{
		if(verifyPin(expanderPin, gpioPin) == 1)
		{
			return gpioPin;
		}
	}
	return 0;
}

int analogShield::verifyPin(int expanderPin, int gpioPin)
{
	#define VERIFICATION_COUNT 5
	int count = 0;
	for(int i = 0; i < VERIFICATION_COUNT; i++)
	{
		count += testPin(expanderPin, gpioPin);
	}

	if(count >= 4)
	{
		return 1;
	}
	return 0;

}
//returns 0 if shieldPins are not connected, 1 if they are
int analogShield::testPin(int expanderPin, int gpioPin)
{
	pinMode(expanderPin, INPUT);
	gpioSelectMode(gpioPin);
	gpioPinMode(gpioPin, OUTPUT);
	gpioWrite(gpioPin, HIGH);
	delay(5);
	if(digitalRead(expanderPin) == HIGH)
	{
		gpioWrite(gpioPin, LOW);
		delay(5);
		if(digitalRead(expanderPin) == LOW)
		{
			delay(5);
			return 1;
		}
		return 0;
	}
	return 0;
}

//returns 0 or 1 for adcbusy, returns -1 for all other shieldPins, returns -2 for error.
int analogShield::shieldPinReadWrite(int shieldPin, int value)
{
	if(shieldPin == adccs)
	{
		if(shieldPins.adccsPin == 0)
		{
			digitalWrite(adccs, value);
		}
		else
		{
			gpioWrite(shieldPins.adccsPin, value);
		}
		return -1;
	}
	if(shieldPin == adcbusy)
	{
		if(shieldPins.adcbusyPin == 0)
		{
			return digitalRead(adcbusy);
		}
		else
		{
			return gpioRead(shieldPins.adcbusyPin);
		}
	}
	if(shieldPin == daccs)
	{
		if(shieldPins.daccsPin == 0)
		{
			digitalWrite(daccs, value);
		}
		else
		{
			gpioWrite(shieldPins.daccsPin, value);
		}
		return -1;
	}
	if(shieldPin == dacld)
	{
		if(shieldPins.dacldPin == 0)
		{
			digitalWrite(dacld, value);
		}
		else
		{
			gpioWrite(shieldPins.dacldPin, value);
		}
		return -1;
	}

	return -2;
}