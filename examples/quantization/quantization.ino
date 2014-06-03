/************************************************************************/
/*                                                                      */
/*  quantization.ino  --  Quantization Demo                             */
/*                                                                      */
/************************************************************************/
/*  Author:  William Esposito                                           */
/*  Copyright 2013, Stanford University                                 */
/************************************************************************/
/*
  This program is free software; you can redistribute it and/or
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
/*  Module Description:                                                 */
/*                                                                      */
/*  Quantization will read on channel 0 and write on channels 0-3 with  */
/*  varying degrees of bit masking.                                     */
/*  EE122A Stanford University.                                         */
/*                                                                      */
/************************************************************************/
/*  Revision History:                                                   */
/*                                                                      */
/*  09/01/2013 (W. Esposito): created                                   */
/*  05/28/2014 (MWingerson): Updated for ChipKIT and release            */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*  Board Support:                                                      */
/*                                                                      */
/*      Arduino UNO                                                     */
/*      ChipKIT UNO32                                                   */
/*      ChipKit UC32                                                    */
/*                                                                      */
/************************************************************************/

#include <analogShield.h>   //Include to use analog shield.
#include <SPI.h>	//required for ChipKIT but does not affect Arduino

void setup()
{
  //no setup
}

unsigned int value, Out0, Out1, Out2, Out3;
void loop()
{
 //Read the signal in from port INO, store in 'value'
 value = analog.read(0);
 // Output quantization:
 // "AND" the 16-bit data with masks containing ONES for bits of the original that will pass through
 // while ZEROES for bits remove information for those bits, effectively reducing the ADC resolution
 Out0 = value;  // Out 0 = 16 bit, full resolution (2^16 = 65,536 quantization levels)
 Out1 = value&0xFC00;  // Out 1 = 6 bit (2^6 = 64 quantization levels)
 Out2 = value&0xF000;  // Out 2 = 4 bit (2^4 = 16 quantization levels)
 Out3 = value&0xC000;  // Out 3 = 2 bit (2^2 = 4 quantization levels)
 //write the data back out! Remember, writing 4 channels out takes nearly 4x as long!
 analog.write(Out0,Out1,Out2,Out3, true); //takes ~50uS
 //analog.write(Out1); //faster one channel -- reading is the longer part, but this only takes 30uS
}






