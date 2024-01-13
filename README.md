# EAAA-public/Teensy-IR-Convolution-Guitar-Pedal
![img missing](https://raw.githubusercontent.com/ThommyJensen/EAAA-public/Teensy-IR-Convolution-Guitar-Pedal/front%20page.png)
  
Impulse response reverb guitar pedal using FFT convolution

Convolution library by Brian Millier https://github.com/bmillier/Teensy-FFT-Convolution-Filter
Display driver is SH110X by adafruit
SD library is arduino standard
Teensy libraries by Paul Stoffregen and others

Software:  
The software allows the user to load one or more impulses from an sd card, though it's inadvisable to go above two.
The userinterface consists of an oled display and a button, which in the case of a finished product would be a monentary
foot switch placed a reasonable distance from the bypass switch.
It is also possible to use an empty response if you just want the surprisingly nice distortion the DAC produces.
The software auto detects type of file and will give useable errors, if the file isn't 16bit, PCM, mono wave file.
.raw files are also permitted, but use those at your own risk. At attempt to allow 32bit float files was also attempted,
but it seems they aren't stored in a format usable by the arduino SD library. Could be compression, endianness, or something else.
Example:
https://youtu.be/Zm_i9kC3oEk

Display  
![alt text](https://raw.githubusercontent.com/ThommyJensen/EAAA-public/Teensy-IR-Convolution-Guitar-Pedal/mainImg.jpg)

Delay is measured between 6 and 7 secs.  
![alt text](https://raw.githubusercontent.com/ThommyJensen/EAAA-public/Teensy-IR-Convolution-Guitar-Pedal/delay.png)


My impulse responses can be found in their respective folder
Example video of them: [https://www.youtube.com/watch?v=rtive3m2jmM&list=PL1sbzfK5jrqY0XgC5ed2O-zZGCqtP8Gmu&index=4](https://youtu.be/rtive3m2jmM?si=7k3oKwE1Ri7NcK0E)
  
  
  
  
HARDWARE:  
![alt text](https://raw.githubusercontent.com/ThommyJensen/EAAA-public/Teensy-IR-Convolution-Guitar-Pedal/Schematic_teensypedal_2023-12-17.png)
Preamp:  
Preamp consists of a standard JFET opamp TL072 used in many guitar pedals.
The first stage handles amplifications from the guitars +-0.3V to the teensy audio adapter's +-1.5V ADC input.
The amplification is done with a voltage divider to the inverting input, with an initial resistor equal to the pot's size,
a coupled resistor to ground to prevent voltage amplification above 100x or 40dB, and the pot's washer to the inverting input.
This is one of the better ways of getting a nice and smoother amplification without inverse log potentiometers, as these aren't
as readily available as linears and logs.

Between the stages is the voltage clamp, that clamps voltage to be between +-1.8V to prevet damaging overvoltage,
using a BAT54s dual schottky diode, that is connected to the teensy's 3v3 rail and ground. A current limiting resistor is used
to ensure proper function of the diode, and minimise it's forward voltage. Peaks above 3333 Hz at currents below 100nA, 
have a forward voltage of 240 mV, according to the BATC54s datasheet.
This could also be done more precisely by adding a trimpot after the clamp, or simply using the opamps rails as max values.
Second stage of the opamp is used as a buffer.

peripherals:  
The gain pot as mentioned is available. On top of this, there's jacks for I/O, a pot for the effect strength, a button
and a largely redundant volume pot.


All videos available at:
https://www.youtube.com/playlist?list=PL1sbzfK5jrqY0XgC5ed2O-zZGCqtP8Gmu

Power:  
A 5V regulator is used to power the teensy's 5V to 3V3 regulator. A cooling element is required to keep at a temp that doesn't burn your fingers.
In case of a normal guitar pedal, you would use the box for cooling as those are typically of the hammond variety, thick die-cast aluminium.
