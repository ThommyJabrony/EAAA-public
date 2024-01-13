bool SDChk () // util for checking for SD card
{
  if (!(SD.begin(10))) 
  {
    Serial.println("Unable to access the SD card");
    return 1;     
  }
  else return 0;
}



class Debouncer		//I couldn't be bothered to try understanding and using someone else's debounce method for push and hold utility, so I wrote my own.
{
  private:
    uint8_t debTime = 50;
    uint16_t timeChk = 500;
    bool state1 = 0;
    long currTime = 0;
    long lastTime = 0;
    long downTime = 0;
    long holdTime = 0;
    bool but_state_curr = 1;
    bool but_state_last = 1;

  public:
    Debouncer(uint16_t compTime)
     {
      timeChk = compTime; //kunne blive til if(compTime) for at tillade void parameter
     }
    uint8_t debounceChk()
    {
      currTime = millis();                //current time
      but_state_curr = digitalRead(selektor); //checks whether button is up or down
      uint8_t returnable = 0;

      if(but_state_last == but_state_curr) lastTime = currTime;   //resets timer if button states are the same


      switch(state1)      //sub states
      {
          case 0:
            
            if(but_state_last && !but_state_curr && currTime - lastTime > debTime)     //checks if but down and prev is up and if debounce time is exceeded
            {
              but_state_last = but_state_curr;                                    //updates prev state
              downTime = millis();                                                //starts timer for press and hold
              
              state1 = 1;
            }
            
            break;
          
          case 1:

            if(!but_state_last && but_state_curr && currTime - lastTime > debTime)    //if last down and current up and debounce time is exceeded
            {
              but_state_last = but_state_curr;
              holdTime = millis();
              if (holdTime - downTime < timeChk)                                    //if the downtime is shorter than .5s, the file is shifted to the next one
              {
                Serial.print("holdtime: ");
                Serial.println(holdTime - downTime);
                returnable = 1;
                Serial.print("returnable = ");
                Serial.println(returnable);
                state1 = 0;
              }
              else                                                     //if held long enough, else is broken
              {
                Serial.print("holdtime: ");
                Serial.println(holdTime - downTime);
                returnable = 2;
                Serial.print("returnable = ");
                Serial.println(returnable);
   
                state1 = 0;
              }
              
              
            }
          
            break;   
          
      }
      
         return returnable;  
    }
};




void setupSGTL5000() {
	// Enable the audio shield. select input. and enable output
	codec.enable();
	codec.adcHighPassFilterDisable(); 
	codec.inputSelect(AUDIO_INPUT_LINEIN); // AUDIO_INPUT_MIC
	codec.lineInLevel(0);	// Changed from library standard, to accomodate full voltage swing of the ADC, as it's used with a preamp. Noticeable drop in noise. May be lowered to have earlier clip as a warning for overvoltage. 
  codec.lineOutLevel(13);	// Changed from library standard, to match input level and permit full voltage swing of DAC. Noticable drop in noise level.
	codec.volume(0.1); //   does affect headphone volume
	// line input sensitivity
	//  0: 3.12 Volts p-p
	//  1: 2.63 Volts p-p
	//  2: 2.22 Volts p-p
	//  3: 1.87 Volts p-p
	//  4: 1.58 Volts p-p
	//  5: 1.33 Volts p-p
	//  6: 1.11 Volts p-p
	//  7: 0.94 Volts p-p
	//  8: 0.79 Volts p-p
	//  9: 0.67 Volts p-p
	// 10: 0.56 Volts p-p
	// 11: 0.48 Volts p-p
	// 12: 0.40 Volts p-p
	// 13: 0.34 Volts p-p
	// 14: 0.29 Volts p-p
	// 15: 0.24 Volts p-p
}



