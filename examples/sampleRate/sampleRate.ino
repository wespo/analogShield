/*
Sample rate will read in a signal in on channel IN0 and write it out on channels OUT0-OUT3 at various sample rates,
representing Full speed, half speed, quarter speed, and one-eigth speed.
This Demo is only compatible with the Arduino Uno and other AVR processors as it uses inline assembly.
EE122A Stanford University
W. Esposito
V1.0 9/2013
*/
#include <analogShield.h>   //Include to use analog shied.

void setup()
{
  //no setup
}

unsigned int count = 0;
unsigned int ucount0 = 0,ucount1 = 0,ucount2 = 0,ucount3 = 0;
int indexSample = 0;
void loop()
{
  cli();
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
  sei();
}