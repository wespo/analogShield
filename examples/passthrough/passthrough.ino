/*
The passthrough routine will simply output the vaule
read by the analogShield input IN0 and put the same
value out on output OUT0.
V1.0 9/2013 W. Esposito
*/
#include <SPI.h> //Required for chipKit uno32, not necessary for arduino Uno / mega
#include <analogShield.h>   //Include to use analog shied.

void setup()
{
  analog.begin(); //required for chipKit uno32, not necessary for arduino Uno / Mega
}

unsigned int count = 0;
void loop()
{
  count = analog.read(0);  //read in on port labeled 'IN0'
  analog.write(0, count);  //write out the value on port labeled 'OUT0'
}