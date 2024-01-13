#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c  //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED'
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)


class DispHandler
{
  private:
    Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

  public:

    DispHandler()
  {
    if (!display.begin(i2c_Address, 0x3C)) 
    {  // Address for 128x64
        Serial.println(F("SH1106 allocation failed"));
        for (;;)
          ;  // Don't proceed, loop forever
    }
  }


    void printIntro(void)       //goes through a series of text displays
    {
      display.clearDisplay();
      display.setTextSize(3);              // Normal 1:1 pixel scale
      display.setTextColor(SH110X_WHITE, SH110X_BLACK);  // Draw white text
      
      
      char name[] = "0lefant";
      char name1[] = "audio";
      
      display.setCursor(0, 10);            // Start at top-left corner
      display.println(name);
      //display.println(F("öLEFANT"));
      display.display();

      delay(1000);
      display.setCursor(0, 40);            // Start at top-left corner
      display.println(name1);
      display.display();
      
      delay(2500);
      display.clearDisplay();
      display.display();
      display.setTextSize(2);              // Normal 1:1 pixel scale
      display.setCursor(0, 0);             // Start at top-left corner
      display.println(F("SELECT"));
      display.display();
      delay(1000);
      display.setCursor(0, 40);            // Start at top-left corner
      display.println(F("IMPULSE"));
      display.display();
      
      
      delay(2500);
      display.clearDisplay();
      display.display();
      delay(300);
      display.setCursor(0, 0);             // Start at top-left corner
      display.println(F("filename: "));
      display.display();
      //delay(300);
      display.clearDisplay();
    }

    void printFilename(String name)               //prints "filename" and a string of the file name
    {
      display.clearDisplay();
      display.setTextColor(SH110X_WHITE, SH110X_BLACK); // Draw white text
      display.setTextSize(2);              // Normal 1:1 pixel scale      
      display.setCursor(0, 0);             // Start at top-left corner
      display.println(F("filename: "));
      
      display.setCursor(0,32); // Start at top-left corner
      display.println(name);
      display.display();
      display.clearDisplay();
  }

  void next(bool yesnt)               //prints "filename" and a string of the file name
    {
      display.clearDisplay();
      display.setTextColor(SH110X_WHITE, SH110X_BLACK); // Draw white text
      display.setTextSize(2);              // Normal 1:1 pixel scale      
      display.setCursor(0, 0);             // Start at top-left corner
      display.println(F("Stack more\nResponses?"));
      
      display.setCursor(0,40); // Start at top-left corner
      if(yesnt) display.setTextColor(SH110X_WHITE, SH110X_BLACK); // Draw white text
      else display.setTextColor(SH110X_BLACK, SH110X_WHITE); // Draw black text 
      display.println(F("NO"));
      
      display.setCursor(64,40); // Start at top-left corner
      if(!yesnt) display.setTextColor(SH110X_WHITE, SH110X_BLACK); // Draw white text
      else display.setTextColor(SH110X_BLACK, SH110X_WHITE); // Draw black text 
      display.println(F("YES"));

      display.display();
      display.clearDisplay();
  }

  void nowPlaying(String name)
  {
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE, SH110X_BLACK); // Draw white text
    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Now \n playing: "));
    display.setCursor(0,32); // Start at top-left corner
    display.println(name);
    display.display();
    display.clearDisplay();
  
    
  }


void error(uint8_t errType)
  {
    if (errType == 0) Serial.println("SD error");
    if (errType == 1) Serial.println("Unknown filetype");
    if (errType == 2) Serial.println("Error in header pos20, file not 16 bit PCM");
    if (errType == 3) Serial.println("Error in header pos22, file not MONO"); 
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE, SH110X_BLACK); // Draw white text
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setCursor(0, 16);             // Start at top-left corner
    if (errType == 0) display.println(F("Error #0:\nSD inaccessible"));
    if (errType == 1) display.println(F("Error #1\nIn header bits 8-11:\nUnknown filetype"));
    if (errType == 2) display.println(F("Error #2\nIn header bit20/34:\nfile not 16bit PCM"));
    if (errType == 3) display.println(F("Error #3\nIn header bit22:\nfile not mono"));
    display.display();
    EEPROM.update(eepAddr, 0);  //Sets value to 0 to prevent intro playing next cycle
    while(1) if(!(digitalRead(selektor))) SCB_AIRCR = 0x05FA0004;   //reset teensy
  }




























/*****************************************************************************SUPER DUPER SECRET DON'T LOOK****************************************************************************/
/*****************************************************************************SUPER DUPER SECRET DON'T LOOK****************************************************************************/
/*****************************************************************************SUPER DUPER SECRET DON'T LOOK****************************************************************************/





















//you were warned
  void theMessage()                              
  {
    for(int x = 0; x < 3 ; x++)
    {
      display.clearDisplay();
      display.setTextColor(SH110X_WHITE, SH110X_BLACK); // Draw white text
      display.setTextSize(5);
      display.setCursor(0, 10);            // Start at top-left corner
      display.println(F("NICE"));
      display.display();

      delay(700);
      display.clearDisplay();
      display.setCursor(0, 10);            // Start at top-left corner
      display.println(F("TRY"));
      display.display();

      delay(700);
    }
    
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 30);            // Start at top-left corner
      display.println(F("no impulse for you :)"));
      display.display();
      delay(800);
      
  }
};