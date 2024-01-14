

const int16_t arrLen = 18001; //array length with padding
float impulse_response[arrLen];       //defining impulse array and int array for 16 bit signedint file


class Loader
{
  private:
    uint16_t oldLen = 1;
    uint8_t multi = 0;
    
    uint8_t typeFetch(std::string name, File datafile)
    {
      std::string wave0 = ".wav";
      std::string wave1 = ".WAV";
      std::string raw0  = ".raw";
      std::string raw1  = ".RAW";
      char header [45];
      uint8_t type = 20;

      if (name.find(wave0) != std::string::npos || name.find(wave1) != std::string::npos) //c++ magic ig
      {
        datafile.read(header, 44); //reads header in chars
        for (int x = 0; x < 44; x++)  
        {
          Serial.print(x); Serial.print(": "); 
          Serial.println(header[x], HEX);    //prints header
        }
          
          
        if (header[8] == 0x57 && header[9] == 0x41 &&     
            header[10] == 0x56 && header[11] == 0x45)     //checks for "WAVE" in header)       //checks for PCM and mono
        {
          Serial.println("wave detected");

          if(header[20] == 0x1 && header[34] == 16) //Checks for PCM and 16 bits of depth 
          {
            Serial.println("File is 16bit PCM");
            
            if(header[22] == 0x1)                   //Confirms channel count as single or mono
            {
              Serial.println("File is mono");
              type = 1;
            } else type = 12;
          } else type = 11;
        }
      }   
      else if (name.find(raw0) != std::string::npos || name.find(raw1) != std::string::npos)  type = 0; //RAW
      else type = 10; //Unknown filetype
        
      
      return type;
        
    }
    

  public:
    Loader()
    {
      impulse_response[0] = 1; //Necessity, as empty response and initial delays in room responses otherwise choke the direct sound
      impulse_response[1] = 1; //Necessity, as empty response and initial delays in room responses otherwise choke the direct sound
      impulse_response[2] = 1; //Necessity, as empty response and initial delays in room responses otherwise choke the direct sound
      for (int x = 3; x < arrLen; x++) impulse_response[x] = 0.0; //zero padding
    }


    File fileFetch (File root)//File datafile)//(bool next, File datafile) //function the returns an object how cool
    { 
      if (root.available())   //checks if SD folder is defined
      {
          File datafile = root.openNextFile();        //does what it says
          if(!datafile) 
          {
            root.rewindDirectory();                   //starts over when data file can't be defined due to reaching end
          }
          Serial.println(datafile.name());
          return datafile;

      }
      
    }


/********************************/


    int floater(File datafile)//, bool first)     //"floates int16 wav PCM/raw file to normalised float array
    {                              //Range +/-15bit int converted to -1 to +1 Float

      if (datafile)   // if file is open
      {
        int len; //impulse length
        
        float mix = analogRead(mixPot); //Recording mix
        mix = map(mix, 0.0,1023.0,0.0,1.0);                    //remap from int10 to 0 - 1 Float32
        Serial.print("mix: ");
        Serial.println(mix);
        
        
        Serial.println("SD card OK");
        Serial.println("file found");
        std::string name  = datafile.name();
        
        int16_t *SD_response = new int16_t[arrLen]; //dynamic array that can be deleted
        
        uint8_t type = typeFetch(name, datafile); //0: unknown, 1: Raw int16, 2: wave int16, 3: wave int16 stereo, 4: wave F32, 5 wave F32 stereo
        
        
          

        if(type == 10) return(1);
        if(type == 11) return(2);
        if(type == 12) return(3);

        
        
        Serial.print("type detected, type: ");
        Serial.println(type);
        

        if (type ==  1)      len = (datafile.size()-44)/2;       // int16 wav
        
        else if (type ==  0) len =  datafile.size()/2;           // int16 raw
        

        if(len  > arrLen) //checks if file is too large for memory to handle
        {
          Serial.print("len too large: "); //error msg
          Serial.println(len);
          Serial.print("will be truncated to 0.4s");
          len = arrLen;     //truncates file to 18000 samples
        } else 
        {
          Serial.print("len: ");
          Serial.println(len);
        }
      
        if(type == 1) datafile.seek(44);
        if(type == 0) datafile.seek(0);
        
        datafile.read(SD_response, len*2); //reads appropriate amount of data from file in bytes(not ints)
        
        datafile.seek(0); //reset datafile, may be redundant
        
        Serial.print("mix: ");
        Serial.println(mix);
    
        
        
        float temp;
        if(!multi)
        {
          for(int x = 3; x < len; x++) 
          {
            temp = SD_response[x];
            temp *= mix;
            temp /= 32768;
            impulse_response[x] = temp;
            oldLen = len;
            multi = 1;   
          }
          Serial.println("first impulse loaded");
        } 
        else
        { 
          if(len > oldLen)
          {
            Serial.println("oldLen smaller");
            for (int x = 3; x < oldLen; x++)
            {
              temp = SD_response[x];
              temp *= mix;
              temp /= 32768;
              if (   impulse_response[x-1] + impulse_response[x] + impulse_response[x+1] < 0.0001                  // checks for changes to counter an initial delay of a room response, that would otherwise choke a cab response
                  && impulse_response[x-1] + impulse_response[x] + impulse_response[x+1] > -0.0001                //the numbers themselves and the shifts x-1 to x+1 are arbitrary 
                  && SD_response[x-1] + SD_response[x] + SD_response[x+1] < 32
                  && SD_response[x-1] + SD_response[x] + SD_response[x+1] > -32) impulse_response[x] += temp;  
              //if (impulse_response[x] > -0.0001 && impulse_response[x] < 0.0001) impulse_response[x] += temp;   // if the previous response is too low, the current is added on top. Might create artifacting, but prevents a cab response from being muted by a room
              else impulse_response[x] = (impulse_response[x] + temp)/2;     //response is averaged with prev response
              //else impulse_response[x] *= temp;   //response is multiplied with previous response
            }
            for (int x = oldLen; x < len; x++)
            {
              temp = SD_response[x];
              temp *= mix;
              temp /= 32768;
              impulse_response[x] = temp;   //Part of response that is further out the reverb than the previous response is used as is
            }
            oldLen = len;
          }
          else 
          {
            Serial.println("newLen smaller");
            for (int x = 3; x < len; x++)
            {
              temp = SD_response[x];
              temp *= mix;
              temp /= 32768;
              if (   impulse_response[x-1] + impulse_response[x] + impulse_response[x+1] < 0.0001
                  && impulse_response[x-1] + impulse_response[x] + impulse_response[x+1] > -0.0001
                  && SD_response[x-1] + SD_response[x] + SD_response[x+1] < 32
                  && SD_response[x-1] + SD_response[x] + SD_response[x+1] > -32) impulse_response[x] += temp;     
              //if (impulse_response[x] > -0.0001 && impulse_response[x] < 0.0001) impulse_response[x] += temp;   
              else impulse_response[x] = (impulse_response[x] + temp)/2;     //response is averaged with prev response
              //else impulse_response[x] *= temp;   //response is multiplied with previous response
            }
          }
          Serial.print("secondary impulse loaded");
        }
        //}

        Serial.println("");
        Serial.print("using impulse:");                 //prints whole impulse
        Serial.println(datafile.name());
        for (int x = 0; x < oldLen; x+=100) Serial.println(impulse_response[x],4);
        Serial.println("");
          
        delete [] SD_response;                          //del int array to clear space
                
        datafile.close();
        return 0;
      }
     
      
    }
    int emptyResponse()
    {
      for (int x = 1; x < arrLen; x++) impulse_response[x] = 0.0; //zero
      Serial.print("using impulse:");                 //prints whole impulse
      Serial.println("clean");
      for (int x = 0; x < arrLen; x+=100) Serial.println(impulse_response[x]);
      impulse_response[0] = 1;
      Serial.println("");
      return 0;
    }
};










