#ifndef UTILS_H
void updChecker()
{
 if(millis() - serialCheck > 50)
  {
      effectThresh = analogRead(effectPin);
      
      selector = digitalRead(selectPin);
      if(selector  != prevSel)
      {
        switcher++;
        if (switcher > switcherMax) switcher = 0;
        Serial.print("comp = ");Serial.println(compare);
        delay(20);
      }
      prevSel = selector;
      

      /*
      Serial.println("status: OK");     //DEBUGGING: LEAVE COMMENTED FOR NORMAL OPERATION
      
      
      Serial.print("comp,  thresh, switcher = ");Serial.println(compare); Serial.println(effectThresh); Serial.println(switcher);
      Serial.println("");
      serialCheck = millis();
      */
      
      serialCheck = millis();
  }
}





float safeSetter(float In)
{
  if(In > 4096) In = 4096;
  else if(In < 0) In = 0;
  return In;
  
}

float crush (float input)
{
  int output;
    input = input / (effectThresh/127);
    output = (int)input  * (int)(effectThresh/127);
    output = safeSetter(output);
  return output;
}

float rect1 (float input)
{
  if(input < 2047) input = 2047;
  
  return input;
  
}
float rect2 (float input)
{
  if(input < 2047) input = 4096-input;
  
  return input;
  
}

float delayer(float input)
{
  float decay = effectThresh/12500 + 0.6; //Decay val from 0.6 to 0.95
  //buffet[idx] *= decay;
  /*
  float sum = input + buffet[idx];
  
  input = sum / ((1+buffet[idx]) / sum);
  //input = (input + buffet[idx]) / ((1+buffet[idx])/(buffet[idx]+input));  //input is mixed with value from buffer that accounts for buffer volume
  */
  input = (input + (buffet[idx]*decay)) / (1+(buffet[idx]*decay)/(((buffet[idx]*decay)+input)));  //input is mixed with value from buffer that accounts for buffer volume
  buffet[idx] = input;
  compare = effectThresh * 4.88;          //Compare match value limits buffer length
  if(idx >= compare) idx = 0;
  else               idx  ++;

  //if(Serial.available()) Serial.print("idx = ");Serial.println(idx); //debug

  return input;
}

float folderHalf(float input)
{
  float threshHi = 3072 + effectThresh/4;
  float threshLo = 1024 - effectThresh/4;
  if (input > threshHi) input = threshHi - (input - threshHi);
  if (input < threshLo) input = threshLo + (threshLo - input);

  return input;
}

float folderWhole(float input)          //THAT NEW SHIT
{
  float threshHi = 2048 + effectThresh/2;                         //Upper threshold
  float threshLo = 2047 - effectThresh/2;                         //Lower threshold
  if (input > threshHi) input = threshHi - (input - threshHi);    //inverts input above threshold
  if (input < threshLo) input = threshLo + (threshLo - input);    //inverts input below threshold

  return input;
}

float clipper(float input)          //THAT NEW SHIT
{
  float threshHi = 4096 - effectThresh/2;                         //Upper threshold
  float threshLo = effectThresh/2;                         //Lower threshold
  if (input > threshHi) input = threshHi;    //clip
  if (input < threshLo) input = threshLo;    

  return input;
}



#endif
