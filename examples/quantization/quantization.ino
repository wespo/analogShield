/*
Quantization will read on channel 0 and write on channels 0-3 with varying degrees of bit masking.
EE122A Stanford University
W. Esposito, Greg Kovacs
V1.0 9/2013
*/
#include <SPI.h> //Required for chipKit uno32, not necessary for arduino Uno / mega
#include <analogShield.h>   //Include to use analog shied.

void setup()
{
  analog.begin(); //required for chipKit uno32, not necessary for arduino Uno / Mega
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






