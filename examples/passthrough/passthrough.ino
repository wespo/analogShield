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
/*  Module Description:                                         */
/*                                                      */
/*  The passthrough routine will simply output the value read by the    */
/*  analogShield input A0 and put the same value out on output D0.      */
/*                                                                      */
/*  Example: Attaching an analog signal to A0 will produce the same     */
/*  analog signal on D0.                                                */
/*                                                      */
/************************************************************************/
/*  Revision History:                                     */
/*                                                      */
/*  09/01/2013 (W. Esposito): created                                   */
/*  05/28/2014 (MWingerson): Updated for ChipKIT and release            */
/*  05/12/2015 (W. Esposito): refactor for DSP Shield                   */                                    */                  
/************************************************************************/

/************************************************************************/
/*  Board Support:                                          */
/*                                                                      */
/*      DSP Shield                                                      */
/*                                                                      */
/************************************************************************/

#include "SPI.h"
#include "analogShield.h"

//note: for the DSP Shield, a jumper wire must be run from ADCCS to pin 10 of the arduino header
//also, for optimal performance. run a jumper from one of the DSP Shield expander header pins 12-15 to DACCS

void setup()
{
  analog.begin(); 
}
void loop()
{
  analog.write(0, analog.read(0));
}