/*
Support library for Texas Instruments Analog Shield

Supports 4 channel ADC and DAC of Analog shield.
It uses the SPI bus of the 

Copyright 2013 William J. Esposito

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "analogShield.h"
analogShield analog;
#if defined(__PIC32MX__)
	#include <SPI.h>
	
	analogShield::analogShield() {
		//begin();
	}

	void analogShield::begin()
	{
		//ADC pins
		pinMode(adccs, OUTPUT);
		digitalWrite(adccs, HIGH);
		pinMode(adcbusy, INPUT);
		
		//DAC pins
		pinMode(syncPin, OUTPUT);
		pinMode(ldacPin, OUTPUT);
		digitalWrite(syncPin,HIGH);  
		digitalWrite(ldacPin,LOW);
		
		
		SPI.begin();
	}

	void analogShield::end()
	{
		SPI.end();
	}





	unsigned int analogShield::read(int channel, bool mode) {
		// initialize SPI:
		if(shieldMode != 2)
		{
			// initialize SPI:
			SPI.setDataMode(SPI_MODE3);
			shieldMode = 2;
		}

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
		if(channel == 3)
		{
			control = control | B11100000;
		}
		else if(channel == 2)
		{
			control = control | B10100000;
		}
		else if(channel == 1)
		{
			control = control | B11010000;
		}
		else //channel == 0
		{
			control = control | B00010000;
		}

		if(mode) //differential mode active
		{
			control = control & B11111011;
		}
		else
		{
			control = control | B00000100;
		}

		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		digitalWrite(adccs,LOW);
		
		SPI.transfer(control);
		
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
	}

	int analogShield::signedRead(int channel, bool mode) {
		// initialize SPI:
		if(shieldMode != 2)
		{
			// initialize SPI:
			SPI.setDataMode(SPI_MODE3);
			shieldMode = 2;
		}

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
		if(channel == 3)
		{
			control = control | B11100000;
		}
		else if(channel == 2)
		{
			control = control | B10100000;
		}
		else if(channel == 1)
		{
			control = control | B11010000;
		}
		else //channel == 0
		{
			control = control | B00010000;
		}

		if(mode) //differential mode active
		{
			control = control & B11111011;
		}
		else
		{
			control = control | B00000100;
		}

		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		digitalWrite(adccs,LOW);
		
		SPI.transfer(control);
		
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

		//compile the result into a 16 bit integer.
		int result;
		//result = (int)word(high, low);
		result = (int)high<<8;
		result+= low;
		
		//make into an unsigned int for compatibility with the DAC used on the analog shield.
		return result;
	}

	void analogShield::write(int channel, unsigned int value) {

		if(shieldMode != 1)
		{
			// initialize SPI:
			SPI.setBitOrder(MSBFIRST);
			SPI.setDataMode(SPI_MODE1);
			SPI.setClockDivider(SPI_CLOCK_DIV2);
			shieldMode = 1;
		}
	  
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value;//lowByte(value);

		// take the SS pin low to select the chip:
		digitalWrite(syncPin,LOW);  
		digitalWrite(ldacPin,LOW);
		
		//  send in the address and value via SPI:
		byte call = 0x10;
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

		//send command byte
		SPI.transfer(call);

		
		//send data
		SPI.transfer(high);
		SPI.transfer(low);
		// take the SS pin high to de-select the chip:
		digitalWrite(syncPin,HIGH);  
		//end();
	}

	void analogShield::writeNoUpdate(int channel, unsigned int value)
	{
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		digitalWrite(5,LOW);
		
		//  send in the address and value via SPI:
		byte call = 0x00;//00
		if(channel == 1)
		{
		call = 0x02;
		}
		else if(channel == 2)
		{
		call = 0x04;
		}
		else if(channel == 3)
		{
		call = 0x06;
		}

		//send command byte
		SPI.transfer(call);

		
		//send data
		SPI.transfer(high);
		SPI.transfer(low);

		// take the SS pin high to de-select the chip:
		digitalWrite(5,HIGH);  
	}

	void analogShield::writeAllUpdate(int channel, unsigned int value)
	{
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		digitalWrite(5,LOW);

		//  send in the address and value via SPI:
		byte call = 0x20;//20
		if(channel == 1)
		{
		call = 0x22;
		}
		else if(channel == 2)
		{
		call = 0x24;
		}
		else if(channel == 3)
		{
		call = 0x26;
		}

		//send command byte
		SPI.transfer(call);

		
		//send data
		SPI.transfer(high);
		SPI.transfer(low);
		
		// take the SS pin high to de-select the chip:
		digitalWrite(5,HIGH);  
	}


	void analogShield::write(unsigned int value0, unsigned int value1, bool simul)
	{
		if(shieldMode != 1)
		{
			// initialize SPI:
			SPI.setDataMode(SPI_MODE1);
			shieldMode = 1;
		}
		
		writeNoUpdate(0,value0);
		writeAllUpdate(1,value1);
	}
	void analogShield::write(unsigned int value0, unsigned int value1, unsigned int value2, bool simul)
	{
		if(shieldMode != 1)
		{
			// initialize SPI:
			SPI.setDataMode(SPI_MODE1);
			shieldMode = 1;
		}
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeAllUpdate(2,value2);
	}
	void analogShield::write(unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3, bool simul)
	{
		if(shieldMode != 1)
		{
			// initialize SPI:
			SPI.setDataMode(SPI_MODE1);
			shieldMode = 1;
		}
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeNoUpdate(2,value2);
		writeAllUpdate(3,value3);
	}
#else
	#include "Arduino.h"

	analogShield::analogShield() {
		//ADC pins
		pinMode(adccs, OUTPUT);
		digitalWrite(adccs, HIGH);
		pinMode(adcbusy, INPUT);
		
		//DAC pins
		pinMode (syncPin, OUTPUT);
		pinMode (ldacPin, OUTPUT);
		digitalWrite(syncPin,HIGH);  
		digitalWrite(ldacPin,LOW);
		
		begin();
		//digitalWrite(syncPin,LOW);
		shieldMode = 0;
	}




	void analogShield::begin() {

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
	}


	void analogShield::end() {
	  SPCR &= ~_BV(SPE);
	}

	unsigned int analogShield::read(int channel, bool mode) {
		// initialize SPI:
		if(shieldMode != 2)
		{
			// initialize SPI:
			SPCR &= ~(_BV(DORD));
			SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE3;
			SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
			SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			shieldMode = 2;
		}

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
		if(channel == 3)
		{
			control = control | B11100000;
		}
		else if(channel == 2)
		{
			control = control | B10100000;
		}
		else if(channel == 1)
		{
			control = control | B11010000;
		}
		else //channel == 0
		{
			control = control | B00010000;
		}

		if(mode) //differential mode active
		{
			control = control & B11111011;
		}
		else
		{
			control = control | B00000100;
		}

		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		PORTD &= B11111011;	//digitalWriteFast(adccs,LOW);
		
		SPDR = control;		//Shift Out the Control Command.
		while (!(SPSR & _BV(SPIF)));
		
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
	}

	int analogShield::signedRead(int channel, bool mode) {
		// initialize SPI:
		if(shieldMode != 2)
		{
			// initialize SPI:
			SPCR &= ~(_BV(DORD));
			SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE3;
			SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
			SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			shieldMode = 2;
		}

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
		if(channel == 3)
		{
			control = control | B11100000;
		}
		else if(channel == 2)
		{
			control = control | B10100000;
		}
		else if(channel == 1)
		{
			control = control | B11010000;
		}
		else //channel == 0
		{
			control = control | B00010000;
		}

		if(mode) //differential mode active
		{
			control = control & B11111011;
		}
		else
		{
			control = control | B00000100;
		}

		// take the SS pin low to select the chip, transfer the command, then bring the bit back high:
		PORTD &= B11111011;	//digitalWriteFast(adccs,LOW);
		
		SPDR = control;		//Shift Out the Control Command.
		while (!(SPSR & _BV(SPIF)));
		
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
		return result;
	}

	void analogShield::write(int channel, unsigned int value) {

		if(shieldMode != 1)
		{
			// initialize SPI:
			SPCR &= ~(_BV(DORD));
			SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
			SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
			SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			shieldMode = 1;
		}
	  
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		PORTD &= B11011111;	//digitalWriteFast(5,LOW);  

		//  send in the address and value via SPI:
		byte call = 0x10;
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
	}

	void analogShield::writeNoUpdate(int channel, unsigned int value)
	{
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		PORTD &= B11011111;	//digitalWriteFast(5,LOW);
		
		//  send in the address and value via SPI:
		byte call = 0x00;//00
		if(channel == 1)
		{
		call = 0x02;
		}
		else if(channel == 2)
		{
		call = 0x04;
		}
		else if(channel == 3)
		{
		call = 0x06;
		}

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
	}

	void analogShield::writeAllUpdate(int channel, unsigned int value)
	{
		//prep work, break up the into into two 8 bit bytes
		byte high = value >> 8;//highByte(value);
		byte low = value & 0x00FF;//lowByte(value);

		// take the SS pin low to select the chip:
		PORTD &= B11011111;	//digitalWriteFast(5,LOW);

		//  send in the address and value via SPI:
		byte call = 0x20;//20
		if(channel == 1)
		{
		call = 0x22;
		}
		else if(channel == 2)
		{
		call = 0x24;
		}
		else if(channel == 3)
		{
		call = 0x26;
		}

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
	}


	void analogShield::write(unsigned int value0, unsigned int value1, bool simul)
	{
		if(shieldMode != 1)
		{
			// initialize SPI:
			SPCR &= ~(_BV(DORD));
			SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
			SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
			SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			shieldMode = 1;
		}
		
		writeNoUpdate(0,value0);
		writeAllUpdate(1,value1);
	}
	void analogShield::write(unsigned int value0, unsigned int value1, unsigned int value2, bool simul)
	{
		if(shieldMode != 1)
		{
			// initialize SPI:
			SPCR &= ~(_BV(DORD));
			SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
			SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
			SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			shieldMode = 1;
		}
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeAllUpdate(2,value2);
	}
	void analogShield::write(unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3, bool simul)
	{
		if(shieldMode != 1)
		{
			// initialize SPI:
			SPCR &= ~(_BV(DORD));
			SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE1;
			SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV2 & SPI_CLOCK_MASK);
			SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV2 >> 2) & SPI_2XCLOCK_MASK);
			shieldMode = 1;
		}
		writeNoUpdate(0,value0);
		writeNoUpdate(1,value1);
		writeNoUpdate(2,value2);
		writeAllUpdate(3,value3);
	}
#endif