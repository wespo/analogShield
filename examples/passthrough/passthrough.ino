/************************************************************************/
/*                                                                      */
/*  passthrough.ino  --  Passthrough Demo                               */
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
/*  Module Description: 				                                */
/*									                                    */
/*  The passthrough routine will simply output the value read by the    */
/*  analogShield input A0 and put the same value out on output D0.      */
/*                                                                      */
/*  Example: Attaching an analog signal to A0 will produce the same     */
/*  analog signal on D0.                                                */
/*									                                    */
/************************************************************************/
/*  Revision History:							                        */
/*									                                    */
/*  09/01/2013 (W. Esposito): created                                   */
/*  05/28/2014 (MWingerson): Updated for ChipKIT and release            */
/*						                                                */									
/************************************************************************/

/************************************************************************/
/*  Board Support:							                            */
/*                                                                      */
/*      Arduino UNO                                                     */
/*      ChipKIT UNO32                                                   */
/*      ChipKIT UC32                                                    */
/*                                                                      */
/************************************************************************/

#include <analogShield.h>   //Include to use analog shield.
#include <SPI.h>	//required for ChipKIT but does not affect Arduino

void setup()
{
  //no setup
}

unsigned int count = 0;
void loop()
{
  count = analog.read(0);  //read in on port labeled 'IN0'
  analog.write(0, count);  //write out the value on port labeled 'OUT0'
}
