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
#ifndef _analogShield_h_
#define _analogShield_h_

	#include <stdio.h>
	#include <SPI.h>
	#include <core.h>
	
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
		void setChannelAndModeByte(int channel, bool mode);
		
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
