/*
This example will put out a linear ramp on output 0
The 16 bit unsigned integer will roll over from +5V 
Back to -5V.
On a scope, it should look something like:
 -/|/|/|/|/|/|/|/|-
V1.0 9/2013 W. Esposito
*/
#include <SPI.h> //Required for chipKit uno32, not necessary for arduino Uno / mega
#include <analogShield.h>   //Include to use analog shied.

void setup()
{
  analog.begin(); //required for chipKit uno32, not necessary for arduino Uno / Mega
}

unsigned int ramp = 0;
void loop() {
  // put your main code here, to run repeatedly: 
  analog.write(0, ramp);
  ramp++;
}
