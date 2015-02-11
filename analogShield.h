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
#ifndef _analogShield_h_
#define _analogShield_h_

    #if defined(__PIC32MX__)
        #include <WProgram.h>
        #include <inttypes.h>
        #include <SPI.h>

	#elif defined(__AVR__)
		#include <stdio.h>
		#include <Arduino.h>
		#include <avr/pgmspace.h>
		
	#elif defined (__SAM3X8E__)
		#include <stdio.h>
		#include <Arduino.h>
		#include <avr/pgmspace.h>
        #include <SPI.h>

        //define some SPI configs
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

	#else //throw error
        #error "Incorrect processor type selected"
	#endif
		
	#include <stdint.h>
	
	//pins
	#define adccs		2
	#define adcbusy		3
	
	#define syncPin		5
	#define ldacPin		6
	#define DIFF_MODE   true
        
	class analogShield {
	private:
		// SPI Configuration methods
		void writeNoUpdate(int channel, unsigned int value);
		void writeAllUpdate(int channel, unsigned int value);
		int shieldMode;
		void setChannelAndModeByte(byte channel, bool mode);
		
	public:
	
    #if defined(__PIC32MX__)
		volatile uint32_t *ADCCSSet;
		volatile uint32_t *ADCCSClr;
		uint32_t ADCCSPinMask;
	
		volatile uint32_t *syncPinSet;
		volatile uint32_t *syncPinClr;
		uint32_t syncPinPinMask;
		
		volatile uint32_t *ldacPinSet;
		volatile uint32_t *ldacPinClr;
		uint32_t ldacPinPinMask;
	
		volatile uint32_t *ADCBusyPtr;
		uint32_t ADCBusyPinMask;
		
	#elif defined(__SAM3X8E__)
		volatile uint32_t *ADCCSPtr;
		uint32_t SetADCCSPinMask;
		uint32_t ClrADCCSPinMask;
	
		volatile uint32_t *syncPinPtr;
		uint32_t SetsyncPinPinMask;
		uint32_t ClrsyncPinPinMask;
		
		volatile uint32_t *ldacPinPtr;
		uint32_t SetldacPinPinMask;
		uint32_t ClrldacPinPinMask;
	
		volatile uint32_t *ADCBusyPtr;
		uint32_t ADCBusyPinMask;
	
	#endif
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
