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
      
      
      serialCheck = millis();
  }
}

/*
void statusCheck()
{
 if(millis() - serialCheck > 2000)
  {
      Serial.println("status: OK");
      Serial.println(switcher);
      Serial.println(effectThresh);
      Serial.print("comp = ");Serial.println(compare);
      Serial.println("");
      serialCheck = millis();
  }
}
*/



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
  buffet[idx] *= decay;
  input = (input + (buffet[idx]*decay)) / (1+(buffet[idx]*decay)/(((buffet[idx]*decay)+input))); //input is mixed with value from buffer that accounts for buffer volume
  buffet[idx] = input;
  compare = effectThresh * 4.88;          //Compare match value limits buffer length
  if(idx >= compare) idx = 0;
  else               idx  ++;

  //if(Serial.available()) Serial.print("idx = ");Serial.println(idx); //debug

  return input;
}


#endif
