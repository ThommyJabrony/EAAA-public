float guitarIn;   //Used to store input from guitar
float guitarMid;  //Value that is processed
float guitarOut;  //Used exclusively as output

#define inPin 28  //ADC Input from guitar
#define outPin 20 //PWM Output
#define effectPin 27  //ADC effect regulation input
#define selectPin 7   //Pin used to switch effect

bool selector = HIGH;   //selector switch state
bool prevSel = HIGH;    //selector switch previous state
uint8_t switcher = 0;   //determines mode(effect), regulated in by select and prevSel
uint8_t switcherMax = 4;  //determines max amount of selectable effects
float effectThresh;     //adc for effect strength


unsigned long serialCheck = 0;   //peripheral update timer
//unsigned long updCheck = 0;      //debugging
unsigned long readCheck = 0;     //adc timer

float buffet[21000]; //1000 extra positions as redundancy
                     // to account for inaccuracies
                     // in the ADC that regulates
                     // the compare match limit.
int idx = 0;         //index value for the array
int compare;         //compare/match to limit values
                     // used in the array, for shorter
                     // or longer echos/delays/loops

#include "utils.h"   //header with functions


void setup() 
{
  // put your setup code here, to run once:
  pinMode(inPin, INPUT);
  pinMode(effectPin, INPUT);
  pinMode(selectPin, INPUT_PULLUP);
    
  //Serial.begin(115200);      //For debugging
  analogReadResolution(12);
  
}

void loop() //reset uS delay to 50 if issues
{
  // put your main code here, to run repeatedly:
  switch(switcher)
  {
    case 0: // CLEAN
    if(micros() - readCheck > 75)
    {
      guitarIn = analogRead(inPin);
      guitarOut = guitarIn;
      readCheck = micros();
    }
      
      
      break;
    case 1: //BITCRUSH
      
      if(effectThresh > 127) 
      {
        if(micros() - readCheck > effectThresh/10)
        {
        guitarIn = analogRead(inPin);
        guitarOut = crush(guitarIn);
        readCheck = micros();
        }
      }
     else //BITCRUSH BUT CLEAN (THRESH TOO LOW)
        { 
        if(micros() - readCheck > 75)
          {
            guitarIn = analogRead(inPin);
            guitarOut = guitarIn;
            readCheck = micros();        
          }
        }
      break;

    case 2: //DELAY
     if(micros() - readCheck > 75)
          {
        guitarIn = analogRead(inPin);
        guitarOut = delayer(guitarIn);
        readCheck = micros();
          }
      break;
/*
    case 3: //FULL WAVE RECTIFIER
      
      if(micros() - readCheck > 75)
      {
        guitarIn = analogRead(inPin);
        guitarOut = rect2(guitarIn);
        readCheck = micros();
      }
      break;
*/
    case 3: //BITCRUSH DELAY COMBO  EXPERIMENTAL VERSION
  
      if(micros() - readCheck > 250)
      {
        guitarIn = analogRead(inPin);
        guitarMid = crush(guitarIn);
        guitarOut = delayer(guitarMid);
        readCheck = micros();
      }
    
     break;
   
    case 4: //FULL WAVE RECT DELAY COMBO
       if(micros() - readCheck > 75)
       {
         guitarIn = analogRead(inPin);
         guitarMid = rect2(guitarIn);
         guitarOut = delayer(guitarMid);
         readCheck = micros();
       }
       break;
    

/*
    case 5: 
      if(micros() - readCheck > 75)
       {
         guitarIn = analogRead(inPin);
         guitarOut = clipper(guitarIn);
         readCheck = micros();
       }
    break;
*/   

   }
   
    

  
  
  
  updChecker();   //Checks changes in peripherals
 // statusCheck();  //Debugging
  
}



void setup1() 
{
  pinMode(outPin, OUTPUT);
  //analogWriteFreq(40000);    //Freq
  analogWriteFreq(32478);    //Freq EXPERIMENTAL
  analogWriteRange(4095);     //Depth in decimal
}
void loop1() 
{
  analogWrite(outPin, guitarOut);
}
