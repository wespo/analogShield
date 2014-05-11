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

//code borrowed from SPI.h as well
/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */
#ifndef _analogShield_h_
#define _analogShield_h_
	#if defined(__AVR__)
	
		#include <stdio.h>
		#include <Arduino.h>
		#include <avr/pgmspace.h>
			
		#define SPI_CLOCK_DIV4 0x00
		#define SPI_CLOCK_DIV16 0x01
		#define SPI_CLOCK_DIV64 0x02
		#define SPI_CLOCK_DIV128 0x03
		#define SPI_CLOCK_DIV2 0x04
		#define SPI_CLOCK_DIV8 0x05
		#define SPI_CLOCK_DIV32 0x06
		//#define SPI_CLOCK_DIV64 0x07

		#define SPI_MODE0 0x00
		#define SPI_MODE1 0x04
		#define SPI_MODE2 0x08
		#define SPI_MODE3 0x0C

		#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
		#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
		#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

	#endif
	
	//pins
	#define adccs		2
	#define adcbusy		3
	
	#define syncPin		5
	#define ldacPin		6
	#define DIFF_MODE       true
        
	class analogShield {
	private:
		// SPI Configuration methods
		static void writeNoUpdate(int channel, unsigned int value);
		static void writeAllUpdate(int channel, unsigned int value);
		int shieldMode;
		
	public:
		analogShield();
		unsigned int read(int channel, bool mode = false);
		int signedRead(int channel, bool mode = false);
		void write(int channel, unsigned int value); //any one channel
		void write(unsigned int value0, unsigned int value1, bool simul); //channels 0 and 1
		void write(unsigned int value0, unsigned int value1, unsigned int value2, bool simul); //channels 0-3
		void write(unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3, bool simul); //all channels
		static void begin(); // Default
		static void end();		
};
	
extern analogShield analog;
#endif
