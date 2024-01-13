# EAAA-public/Teensy-IR-Convolution-Guitar-Pedal

Impulse response reverb guitar pedal using FFT convolution

Convolution library by Brian Millier https://github.com/bmillier/Teensy-FFT-Convolution-Filter
Display driver is SH110X by adafruit
SD library is arduino standard
Teensy libraries by Paul Stoffregen and others

Software:
to be written

HARDWARE:  
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
