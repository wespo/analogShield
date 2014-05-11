analogShield
============

Analog Shield Library
ADC / DAC library for the Digilent Analog Shield

To include the library in a sketch, once it is installed, it is simply invoked by typing:
```
#include <analogShield.h>
```
Once the shield library is included, the sketch will have access to a variable 
called analog, which has access to all the methods associated with the shield. 
The following are the six most important methods associated with the shield.
```
  unsigned int read(int channel, bool mode = false);
```
This method samples the selected analog input. ‘mode’ is an optional parameter. 
False (the default value) is normal single ended mode where each analog pin is referenced to ground. True enables ‘differential’ mode, where the value returned is the difference between the voltages on pairs of adjacent inputs (i.e. IN0-IN1 and IN2-IN3). Read returns an unsigned integer.
```
  signedRead(int channel, bool mode = false);
```
Signed read behaves identically to read, but returns a signed integer.
```
  write(int channel, unsigned int value);
  write(unsigned int value0, unsigned int value1, true); 
  write(unsigned int value0, unsigned int value1, unsigned int value2, true); 
  write(unsigned int value0, unsigned int value1, unsigned int value2, unsigned int value3, true);
```
These methods allow writing to the Analog Shield’s analog output. 
The first method writes to any arbitrary selected channel. 
The next three methods write to two or more channels simultaneously 
(but don’t allow selection of which channel is to be written). 
The simultaneous update methods are useful for higher speed applications 
where a small delay between channel outputs will produce unwanted signals 
or timing issues. For example, to use the shield to drive the X-Y input 
of an oscilloscope to draw a circle, a delay between writing the X and 
the Y channels produces a ghost ‘ellipse’ drawn during the interim between 
the X and Y DAC updates.
