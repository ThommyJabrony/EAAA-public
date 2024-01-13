/*
Menu handler function
Everything not needed later is encapsulated here to avoid compromising the convolution's memory requirement.
*/
void menu()
{ 
  
  DispHandler disp = DispHandler();
  Loader load = Loader();
  Debouncer debounce = Debouncer(500);
  
  byte eepVal = EEPROM.read(eepAddr);   //loads value from EEPROM address
  if (eepVal)      disp.printIntro();   //If value is set, intro will play
  EEPROM.update(eepAddr, 255);          //Sets value so intro plays if teensy is reset via power cut, but not via button
      
  

  if(SDChk()) disp.error(0); 
  File root = SD.open("/");
  File datafile = load.fileFetch(root);


  String filename;
  uint8_t state = 1;
  bool nextFile = 0;
  bool next = 0;
  bool multi = 0;
  bool clen = 0;
  uint8_t debbie;
  
  /*
	The menu 

  */

  while(state != 0)
  {
    
    delayMicroseconds(50);
    switch (state)
    {
      
      case 1: //fetching name from sd
        if(nextFile) datafile = load.fileFetch(root); //Only opens next file, if nextFile is set                        
        if(datafile)
        { 
          filename = datafile.name();
          disp.printFilename(filename);    //displays datafile
        }
        if(!datafile) disp.printFilename("No\n selection");
        state = 2;
        
        break;
      
      
      case 2: // button unpressed mode
        debbie = debounce.debounceChk();
        if (debbie == 1) 							// clicks
        {
          if(!nextFile) nextFile = 1;                 
          state = 1;
        }
        else if (debbie == 2)                       // holds
        {
          if(!datafile)
          {
            if(multi) disp.theMessage(); // simple error message, don't look into it itself. May to return to menu after displaying error, unlike clean response, which works if done the first time
            
            load.emptyResponse();
            clen = 1;
            state = 0;
          }
          else
          {
            uint8_t floatReturn = load.floater(datafile);
            if(floatReturn > 0) disp.error(floatReturn);
            
            state = 3;
          }
          
        }
        break;

      case 3:
        disp.next(next);
        state = 4;
        break;
      
      case 4:   // load another impulse
        debbie = debounce.debounceChk();
        if (debbie == 1)      
        {
          next = !next;
          state = 3;
        }
        else if (debbie == 2)     //hold 
        {
          if (next)               // if yes is selected
          {
            multi = 1;
            next = 0;
            state = 1;
          }
          else                   //if no selected
          {
            state = 0;
          }
        }
        break;
        
      
    }
    
  }
  Serial.println("while loop broken");
  
  if(clen) disp.nowPlaying("clean/dry"); //if clean variable is set, it will always display clean, as selecting clean response overwrites any previous loaded responses
  else
  {
    if(multi)disp.nowPlaying("multiple");   //if multiple responses are in use
    else disp.nowPlaying(filename);         //if only one response is loaded
  }
  
  if(datafile) datafile.close(); //DATAfile is close if it exists
  root.close();     //same with root
  
  //delete disp;
  //delete load;

  Serial.println("end of menu.");
}

