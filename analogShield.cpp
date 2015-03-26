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
**			for the ADC and DAC control pins 
**
**
*/	
    analogShield::analogShield() {	
		//Initialize Private Variables
		shieldMode = 0;
		//Set up DSP Shield SPI Bus
		//begin();
		
		digitalWrite(ARD_SPI_EN, HIGH);
		digitalWrite(adccs, HIGH);
		digitalWrite(syncPin,HIGH);  
		digitalWrite(ldacPin,LOW);

		SPI.setBitOrder(MSBFIRST);
    	SPI.setDataMode(SPI_MODE1);
    	SPI.setClockDivider(SPI_CLOCK_DIV2);
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
		pinMode(ARD_SPI_EN, OUTPUT);
		pinMode(adccs, OUTPUT);
		pinMode(adcbusy, INPUT);
		pinMode(syncPin, OUTPUT);
		pinMode(ldacPin, OUTPUT);
	
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
		if(result < 0)
		{
			result &= 0x7FFF;
		}
		else
		{
			result |= 0x8000;
		}
		return result;

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
	int analogShield::signedRead(int channel, bool mode) {
		// initialize SPI:
		if(shieldMode != 2){
			SPI.setDataMode(SPI_MODE3);
			shieldMode = 2;
		}
		
		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		digitalWrite(adccs,LOW);
		
		setChannelAndModeByte(channel, mode);
		
		//digitalWrite(adccs,HIGH);
		
		//wait for busy signal to fall. If it lasts a while, try resending.
		while(digitalRead(adcbusy) == 1){} //wait for pin 3 to == 0

		//Result ready. Read it in
		//digitalWrite(adccs,LOW);

		//collect data
		unsigned int readValue[2] = {};
		SPI.read(readValue, 2);
		
		//release chip select
		digitalWrite(adccs,HIGH);
		
		//compile the result into a 16 bit integer.
		int result;
		//result = (int)word(high, low);
		result = readValue[1]<<8;
		result+= readValue[0];
		
		//make into an unsigned int for compatibility with the DAC used on the analog shield.
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
		// take the SS pin low to select the chip:
		digitalWrite(syncPin,LOW);
		
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
		digitalWrite(syncPin,HIGH);  
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
		// take the SS pin low to select the chip:
		digitalWrite(syncPin,LOW);
		
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
		
		// take the SS pin high to de-select the chip:
		digitalWrite(syncPin,HIGH);  

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

		if(shieldMode != 1)
		{
			// initialize SPI:
			SPI.setBitOrder(MSBFIRST);
			SPI.setDataMode(SPI_MODE1);
			SPI.setClockDivider(SPI_CLOCK_DIV2);
			shieldMode = 1;
		}
	  
		// take the SS pin low to select the chip:
		digitalWrite(syncPin,LOW);
				
		//  send in the address and value via SPI:
		unsigned int call = 0x10;
		if(channel == 1)
			call = 0x12;
		
		else if(channel == 2)
			call = 0x14;
		
		else if(channel == 3)
			call = 0x16;
		
		unsigned int message[3] = { call, value >> 8, value & 0x00FF};
  		SPI.write(message, 3);
		// take the SS pin high to de-select the chip:
		digitalWrite(syncPin,HIGH);  
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
		if(shieldMode != 1)
		{
			// initialize SPI:
			SPI.setDataMode(SPI_MODE1);
			shieldMode = 1;
		}
		
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
		if(shieldMode != 1){
			// initialize SPI:
			SPI.setDataMode(SPI_MODE1);
			shieldMode = 1;
		}
		
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
		if(shieldMode != 1){
			// initialize SPI:
			SPI.setDataMode(SPI_MODE1);
			shieldMode = 1;
		}
		
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeAllUpdate(2,value2);
		writeAllUpdate(3,value3);
	}
