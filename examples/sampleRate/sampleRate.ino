/************************************************************************/
/*                                                                      */
/*  sampleRate.ino  --  Sample Rate Demo                                */
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
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
/************************************************************************/
/*  Module Description:                                                 */
/*                                                                      */
/*  Sample rate will read in a signal in on channel IN0 and write it    */
/*  out on channels OUT0-OUT3 at various sample rates,representing Full */
/*  speed, half speed, quarter speed, and one-eigth speed.              */
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

unsigned int count = 0;
unsigned int ucount0 = 0,ucount1 = 0,ucount2 = 0,ucount3 = 0;
int indexSample = 0;
void loop()
{
  noInterrupts();
  count = analog.read(0);
  ucount0 = count;  //every loop, update channel 0;

  if(0 == (indexSample%2))    //every other loop, update channel 1;
  {
    ucount1 = count;  
  }
  else
  {
    __asm__("nop\n\t""nop\n\t""nop\n\t");  //Delay intended to make sure that the 'loop' takes the same amount of time whether or not it updated channel 1.
  }
  if(0 == (indexSample%4))  //every 4th loop, update channel 2;
  {
    ucount2 = count;    
  }
  else
  {
    __asm__("nop\n\t""nop\n\t""nop\n\t");  //Delay intended to make sure that the 'loop' takes the same amount of time whether or not it updated channel 2.
   }
  if(0 == (indexSample%8))  //every 8th loop, update channel 3;
  {
    ucount3 = count;    
  }
  else
  {
    __asm__("nop\n\t""nop\n\t""nop\n\t");  //Delay intended to make sure that the 'loop' takes the same amount of time whether or not it updated channel 3.
  }
  analog.write(ucount0,ucount1,ucount2,ucount3, true);
  indexSample++;
  interrupts();
}
