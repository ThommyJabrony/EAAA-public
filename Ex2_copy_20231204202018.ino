/*
IR CONVOLUTION PEDAL using Brian Millier's convolution library https://github.com/bmillier/Teensy-FFT-Convolution-Filter
Allows loading of single impulse responses (stable) and rudimentary loading of multiple (experimental)
Multi response functionality is done by adding responses together rather than convoluting them together, making an amalgam.
It still works, as when using a short response, like a cab, that fits within the initial delay of a room, the effect of the first response
is convoluted onto the output buffer which is eventually convoluted with the second response.
Warning: not all responses are created equally. If the room/cab/instrument body has resonant frequencies within the frequency range you play through the pedal,
they may amplify the notes very strongly. Example is my box guitar that self oscillates between 110 and 120 Hz Hz, affecting the notes A2, Ab2, and B2.
This issue should be possible to resolve in the software used to make edit the responses, by placing a high order digital filter around the affected frequencies.

This project is unlicenced, so feel free to use it.
IR's could become a bit longer if I rewriting the project with with Frank DD4WH's convolution algorithm, as it's slightly faster than Bryan's.
*/
#include <string>
#include <cstring>


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h> 
#include <arm_math.h>
#include <arm_const_structs.h> // in the Teensy 4.0 audio library, the ARM CMSIS DSP lib is already a newer version
#include <utility/imxrt_hw.h> // necessary for setting the sample rate, thanks FrankB !
#include <EEPROM.h>


uint8_t eepAddr;      //EEPROM address for reset check
#define selektor 0    //button pin
#define mixPot 24     //pot for response strength 



#include "loader.h"
#include "dispHandler.h"

const int nc = 18000; // number of samples for the IR in the time domain. Equates to 408 milisecs, which is enough for a small room.
#include "convolution.h"


// GUItool: begin automatically generated code
AudioInputI2S            	i2s1;           
AudioFilterConvolutionUP    convolution;    // Uniformly-partitioned convolution filter    
AudioOutputI2S           	i2s2;           
AudioConnection          	patchCord1(i2s1, 0, convolution, 0);
AudioConnection          	patchCord2(i2s1, 1, convolution, 1);
AudioConnection          	patchCord3(convolution, 0, i2s2,0);
AudioConnection          	patchCord4(convolution, 1, i2s2,1);
AudioControlSGTL5000     	codec;     
// GUItool: end automatically generated code

#include "utils.h"
#include "menu.h"

void setup() 
{
  Serial.begin(115200);
  Serial.println("setup has begun");
  pinMode(selektor, INPUT_PULLUP);
  
  while(!(digitalRead(selektor))) ;
  menu();
  
  Serial.println("floated");
  Serial.println();
  Serial.println();

  
  //while(!Serial);   // must patch out for Visual Micro
  pinMode(14, OUTPUT);  // digital signal used to measure routine's execution time on 'scope
  setupSGTL5000();
  AudioMemory(10); 
    
  set_arm_clock(600000000);
  setI2SFreq(SAMPLE_RATE);

  convolution.begin(0,0.20,*fftout,nfor); // turn off update routine until after filter mask is generated, set Audio_gain=1.00 , point to fftout array, specify number of partitions
  
  /****************************************************************************************
     Calculate the FFT of the FIR filter coefficients once to produce the FIR filter mask
  ****************************************************************************************/
  convolution.impulse(impulse_response, maskgen, nc);
    

    flexRamInfo();

    Serial.println();
    Serial.print("AUDIO_BLOCK_SAMPLES:  ");     Serial.println(AUDIO_BLOCK_SAMPLES);

    Serial.println();
    
  
}





void loop() {
	yield();

	if(!(digitalRead(selektor)))  //reset function
  {
    EEPROM.update(eepAddr, 0);  //Sets value to 0 to prevent intro playing next cycle
    SCB_AIRCR = 0x05FA0004;     //reset teensy
  }
}

