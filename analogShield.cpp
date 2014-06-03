/************************************************************************/
/*																		*/
/*	analogShield.h	--	Library for Analog Shield	     	            */
/*																		*/
/************************************************************************/
/*	Author: 	William J. Esposito										*/
/*	Copyright 2014, Digilent Inc.										*/
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
/*	forms a chipKIT/Aruino interface to the Analog Shield functions of  */
/*  the Texas Instrument chipKit/arduino shield.						*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	04/22/2014(WilliamE): Created										*/
/*  05/27/2014(MarshallW): Modified for readability and content         */
/*																		*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "analogShield.h"
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
	//initialize the ADC and DAC pins
		//ADC pins
		pinMode(adccs, OUTPUT);
		digitalWrite(adccs, HIGH);
		pinMode(adcbusy, INPUT);
		
		//DAC pins
		pinMode(syncPin, OUTPUT);
		pinMode(ldacPin, OUTPUT);
		digitalWrite(syncPin,HIGH);  
		digitalWrite(ldacPin,LOW);
			
		shieldMode = 0;
		
		begin();
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
	#if defined(__PIC32MX__)		
		SPI.begin();
	#else //(__AVR__)
	  // Set SS to high so a connected chip will be "deselected" by default
	  digitalWrite(SS, HIGH);

	  // When the SS pin is set as OUTPUT, it can be used as
	  // a general purpose output port (it doesn't influence
	  // SPI operations).
	  pinMode(SS, OUTPUT);

	  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
	  // automatically switches to Slave, so the data direction of
	  // the SS pin MUST be kept as OUTPUT.
	  SPCR |= _BV(MSTR);
	  SPCR |= _BV(SPE);

	  // Set direction register for SCK and MOSI pin.
	  // MISO pin automatically overrides to INPUT.
	  // By doing this AFTER enabling SPI, we avoid accidentally
	  // clocking in a single bit since the lines go directly
	  // from "input" to SPI control.  
	  // http://code.google.com/p/arduino/issues/detail?id=888
	  pinMode(SCK, OUTPUT);
	  pinMode(MOSI, OUTPUT);		
	#endif
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
		#if defined(__PIC32MX__)
			SPI.end();
		#else //(__AVR__)
			SPCR &= ~_BV(SPE);
		#endif
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
	void analogShield::setChannelAndModeByte(byte channel, bool mode){		
		//control byte
		//S - sentinel, always 1
		//A2 - channel select
		//A1 - channel select
		//A0 - channel select
		//- N/C
		//Single / Diff pair
		//PD1 - power down mode
		//PD0 - power down mode
		byte control = B10000010; //default to channel 1 '001'
		
		//channel mask
		if(channel == 3){
			control = control | B11100000;
		}
		
		else if(channel == 2){
			control = control | B10100000;
		}
		
		else if(channel == 1){
			control = control | B11010000;
		}
		
		else if(channel == 0){
			control = control | B00010000;
		}
		
		//differential mode active
		if(mode){
			control = control & B11111011;
		}
		
		else{
			control = control | B00000100;
		}
		
		
		#if defined(__PIC32MX__)

		SPI.transfer(control);

		#else //(__AVR__)
			SPDR = control;		//Shift Out the Control Command.
			while (!(SPSR & _BV(SPIF)));
		#endif			
		
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
		// initialize SPI:
		if(shieldMode != 2){
			#if defined (__PIC32MX__)
				SPI.setDataMode(SPI_MODE3);
			#else //(__AVR__)
				SPCR &= ~(_BV(DORD));
				SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE3;
				SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
				SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			#endif
			shieldMode = 2;
		}
		
		#if defined (__PIC32MX__)
		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		digitalWrite(adccs,LOW);
		
		setChannelAndModeByte(channel, mode);
		
		digitalWrite(adccs,HIGH);
		
		//wait for busy signal to fall. If it lasts a while, try resending.
		while(digitalRead(adcbusy) == 0); //wait for pin 3 to == 0

		//Result ready. Read it in
		digitalWrite(adccs,LOW);

		//collect data
		byte high = SPI.transfer(0x00);
		byte low = SPI.transfer(0x00);
		
		//release chip select
		digitalWrite(adccs,HIGH);
				
		//compile the result into a 32 bit integer.
		int result;
			result = (int)high<<24;
		result+= low<<16;
		
		//make into an unsigned int for compatibility with the DAC used on the analog shield.
		if(result < 0)
		{
			result &= 0x7FFFFFFF;
		}
		else
		{
			result |= 0x80000000;
		}
		return result>>16;
		
		#else //(__AVR__)
		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		PORTD &= B11111011;	//digitalWriteFast(adccs,LOW);
		
		setChannelAndModeByte(channel, mode);
		
		PORTD |= B00000100;	//digitalWriteFast(adccs,HIGH);
		
		//wait for busy signal to fall. If it lasts a while, try resending.
		while((PIND & B00001000) == 0); //wait for pin 3 to == 0

		//Result ready. Read it in
		PORTD &= B11111011;	//digitalWriteFast(adccs,LOW);

		//collect data
		SPDR = 0;
		while (!(SPSR & _BV(SPIF)));
		byte high = SPDR;
		SPDR = 0;
		while (!(SPSR & _BV(SPIF)));
		byte low = SPDR;
		
		//release chip select
		PORTD |= B00000100;	//digitalWriteFast(adccs,HIGH);
		
		//compile the result into a 16 bit integer.
		int result;
		//result = (int)word(high, low);
		result = (int)high<<8;
		result+= low;
		
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
		#endif

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
			#if defined (__PIC32MX__)
				SPI.setDataMode(SPI_MODE3);
			#else //(__AVR__)
				SPCR &= ~(_BV(DORD));
				SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE3;
				SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
				SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			#endif
			shieldMode = 2;
		}
		
		#if defined (__PIC32MX__)
		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		digitalWrite(adccs,LOW);
		
		setChannelAndModeByte(channel, mode);
		
		digitalWrite(adccs,HIGH);
		
		//wait for busy signal to fall. If it lasts a while, try resending.
		while(digitalRead(adcbusy) == 0); //wait for pin 3 to == 0

		//Result ready. Read it in
		digitalWrite(adccs,LOW);

		//collect data
		byte high = SPI.transfer(0x00);
		byte low = SPI.transfer(0x00);
		
		//release chip select
		digitalWrite(adccs,HIGH);
		
		#else //(__AVR__)
		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		PORTD &= B11111011;	//digitalWriteFast(adccs,LOW);
		
		setChannelAndModeByte(channel, mode);
		
		PORTD |= B00000100;	//digitalWriteFast(adccs,HIGH);
		
		//wait for busy signal to fall. If it lasts a while, try resending.
		while((PIND & B00001000) == 0); //wait for pin 3 to == 0

		//Result ready. Read it in
		PORTD &= B11111011;	//digitalWriteFast(adccs,LOW);

		//collect data
		SPDR = 0;
		while (!(SPSR & _BV(SPIF)));
		byte high = SPDR;
		SPDR = 0;
		while (!(SPSR & _BV(SPIF)));
		byte low = SPDR;
		
		//release chip select
		PORTD |= B00000100;	//digitalWriteFast(adccs,HIGH);
		#endif
		
		//compile the result into a 16 bit integer.
		int result;
		//result = (int)word(high, low);
		result = (int)high<<8;
		result+= low;
		
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
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		#if defined(__PIC32MX__)
			// take the SS pin low to select the chip:
			digitalWrite(syncPin,LOW);
		#else
			// take the SS pin low to select the chip:
			PORTD &= B11011111;	//digitalWriteFast(5,LOW);
		#endif
		
		//  send in the address and value via SPI:
		byte call = 0x00;//00
		if(channel == 1)
			call = 0x02;
		
		else if(channel == 2)
			call = 0x04;
		
		else if(channel == 3)
			call = 0x06;
		

		//send command byte
		#if defined(__PIC32MX__)
			SPI.transfer(call);
		
			//send data
			SPI.transfer(high);
			SPI.transfer(low);
			
			// take the SS pin high to de-select the chip:
			digitalWrite(syncPin,HIGH); 
			
		#else //(__AVR__)
			SPDR = call;
			while (!(SPSR & _BV(SPIF)));

			//send data
			SPDR = high;
			while (!(SPSR & _BV(SPIF)));
			SPDR = low;
			while (!(SPSR & _BV(SPIF)));
				
			// take the SS pin high to de-select the chip:
			PORTD |= B00100000;	////digitalWriteFast(5,HIGH);  
		#endif
	
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
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		digitalWrite(5,LOW);

		//  send in the address and value via SPI:
		byte call = 0x20;//20
		if(channel == 1)
			call = 0x22;
		
		else if(channel == 2)
			call = 0x24;
		
		else if(channel == 3)
			call = 0x26;
		
		//send command byte
		#if defined(__PIC32MX__)
			SPI.transfer(call);
		
			//send data
			SPI.transfer(high);
			SPI.transfer(low);
			
			// take the SS pin high to de-select the chip:
			digitalWrite(syncPin,HIGH); 
			
		#else //(__AVR__)
			SPDR = call;
			while (!(SPSR & _BV(SPIF)));

			//send data
			SPDR = high;
			while (!(SPSR & _BV(SPIF)));
			SPDR = low;
			while (!(SPSR & _BV(SPIF)));
				
			// take the SS pin high to de-select the chip:
			PORTD |= B00100000;	////digitalWriteFast(5,HIGH);  
		#endif
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
			#if defined(__PIC32MX__)
				SPI.setBitOrder(MSBFIRST);
				SPI.setDataMode(SPI_MODE1);
				SPI.setClockDivider(SPI_CLOCK_DIV2);
			#else //(__AVR__)
				SPCR &= ~(_BV(DORD));
				SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
				SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
				SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			#endif
			shieldMode = 1;
		}
	  
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		#if defined(__PIC32MX__)
			// take the SS pin low to select the chip:
			digitalWrite(syncPin,LOW);  
			digitalWrite(ldacPin,LOW);
		#else //(__AVR__)
			// take the SS pin low to select the chip:
			PORTD &= B11011111;	//digitalWriteFast(5,LOW);  	
		#endif
		
		//  send in the address and value via SPI:
		byte call = 0x10;
		if(channel == 1)
			call = 0x12;
		
		else if(channel == 2)
			call = 0x14;
		
		else if(channel == 3)
			call = 0x16;
		
		#if defined(__PIC32MX__)
			//send command byte
			SPI.transfer(call);

			//send data
			SPI.transfer(high);
			SPI.transfer(low);
			// take the SS pin high to de-select the chip:
			digitalWrite(syncPin,HIGH);  
			//end();
		
		#else //(__AVR__)
			//send command byte
			SPDR = call;
			while (!(SPSR & _BV(SPIF)));

			//send data
			SPDR = high;
			while (!(SPSR & _BV(SPIF)));
			SPDR = low;
			while (!(SPSR & _BV(SPIF)));
			// take the SS pin high to de-select the chip:
			PORTD |= B00100000;	////digitalWriteFast(5,HIGH);  
			//end();
		#endif
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
			#if defined (__PIC32MX__)
				SPI.setDataMode(SPI_MODE1);
			
			#else  (__AVR__)
				SPCR &= ~(_BV(DORD));
				SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
				SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
				SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			#endif
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
			#if defined (__PIC32MX__)
				SPI.setDataMode(SPI_MODE1);
			
			#else  //(__AVR__)
				SPCR &= ~(_BV(DORD));
				SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
				SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
				SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			#endif
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
		if(shieldMode != 1)
		{
			// initialize SPI:
			#if defined (__PIC32MX__)
				SPI.setDataMode(SPI_MODE1);
			
			#else  //(__AVR__)
				SPCR &= ~(_BV(DORD));
				SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
				SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
				SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			#endif
			shieldMode = 1;
		}
		
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeNoUpdate(2,value2);
		writeAllUpdate(3,value3);
	}
