/*******************************************************************************************
 * /                        Arduino Clock with and 20x4 LCD, DS 1307 RTC,
 * /                             3 Push Buttons and Rocker Switch.  
 * /
 * /                            Garth Vander Houwen, July, 2016
 * /                              Last Updated January, 2019
 * /
/********************************************************************************************/
// TODO
// * eeprom settings
// * setting update ui
// * alarm function and settings
// * handle hour rollover screen issue

#define build 1
#define revision 6
#define buzzer 10
#define OCTAVE_OFFSET 0
#define isdigit(n) (n >= '0' && n <= '9')
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h> 
RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// custom characters for time font
byte x10[8] = {0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x07};// digit piece   
byte x11[8] = {0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x1C};// digit piece
byte x12[8] = {0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F};// digit piece
byte x13[8] = {0x07,0x07,0x07,0x07,0x07,0x1F,0x1F,0x1F};// digit piece
byte x14[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1F,0x1F,0x1F};// digit piece
byte x15[8] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C};// digit piece
byte x16[8] = {0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07};// digit piece
byte x17[8] = {0x00,0x00,0x0E,0x0E,0x0E,0x00,0x00,0x00};// digit piece (colon)
// the different states of the clock
enum states
{
    SHOW_SPLASH,         // Displays the Splash Screen
    SHOW_TIME,           // Displays the time and date
    SHOW_CUSTOM_TEXT,    // Displays the time and custom text from the serial port
    SHOW_SYSTEM_INFO,    // Displays System Information
};
// holds the current and previous state of the clock
states state, previousState;  

//////////////////////
// Global Variables //
//////////////////////
// Button Pins
int button1Pin = 2;
int button2Pin = 3;
int button3Pin = 4;
int button4Pin = 5;
// Button States
int button1State, button2State, button3State, button4State; 
int previousToggleState; 
// Time and date variables
const char* dayName[] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
const char* monthName[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
int hr_24, hr_12;
String ampm;
// Time Display options (TODO Store in eeprom)
bool showSeconds = true;
bool militaryTime = false;
// sound playback
const int tonePin = buzzer;  // for rEDI board
// These values can also be found as constants in the Tone library (Tone.h)
int notes[] = { 0, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951};
// alarms
char *nokia = "NokiaTun:d=4,o=5,b=225:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a";
char *hamster_dance = "HamsterD:d=16,o=6,b=140:g#,g,8g#,8e,8e,e,8b5.,4b5,4g#,8b5,g#,8a,2b.,e7,c#7,b,c#7,b,g#,b,g#,f#,e,c#,e,f#,e,g#,b,g#,b,f#,e,8c#,2e";
char *twentyfour_phone = "24Ctu:d=16,o=5,b=565:f6,p,f6,4p,p,f6,p,f6,2p,p,b6,p,b6,p,b6,p,b6,p,b,p,b,p,b,p,b,p,b,p,b,p,b,p,b,1p.,2p.,p,f6,p,f6,4p,p,f6,p,f6,2p,p,b6,p,b6,p,b6,p,b6,p,b,p,b,p,b,p,b,p,b,p,b,p,b,p,c";

///////////////////////
// Setup - runs once //
///////////////////////
void setup() {

    // Used to type in characters in custom text mode
    Serial.begin(9600);
    // Start the RTC 
    Wire.begin();
    RTC.begin();
    // Setup Button Pins
    pinMode(button1Pin, INPUT_PULLUP);
    pinMode(button2Pin, INPUT_PULLUP);
    pinMode(button3Pin, INPUT_PULLUP);
    pinMode(button4Pin, INPUT_PULLUP);
    // Set buzzer as an output
    pinMode(buzzer, OUTPUT); 
    noTone (buzzer);
    // setup LCD  for 20 columns and 4 rows
    lcd.begin(20,4); 
    // create custom characters for clock font
    lcd.createChar(0, x10);                      // digit piece
    lcd.createChar(1, x11);                      // digit piece
    lcd.createChar(2, x12);                      // digit piece
    lcd.createChar(3, x13);                      // digit piece
    lcd.createChar(4, x14);                      // digit piece
    lcd.createChar(5, x15);                      // digit piece
    lcd.createChar(6, x16);                      // digit piece
    lcd.createChar(7, x17);                      // digit piece (colon)
    // set the time if the RTC is not running
    if (! RTC.isrunning()) {
      Serial.println("RTC is NOT running!");
      // the following line sets the RTC to the date & time this sketch was compiled
      RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    // Set Initial state
    state = SHOW_SPLASH;          
    previousState = SHOW_SPLASH;
}

/////////////////////////////////////
// Loop - runs over and over again //
/////////////////////////////////////
void loop() {           
    // Uses the current state to decide what to process
    
    // Clear the LCD if state has changed
    if(previousState != state)
    {
      lcd.clear();
    }
    switch (state)
    {
        case SHOW_SPLASH:
            drawLoadingScreen();
            lcd.clear();
            showSplash();
            delay(6000);
            lcd.clear();
            state = SHOW_TIME;
            break;
        case SHOW_TIME:
            showTime();
            showDate();
            previousState = state;
            break;
        case SHOW_CUSTOM_TEXT:     
            showSerialText();
            showTime();
            previousState = state;
            break;
        case SHOW_SYSTEM_INFO:  
            showSystemInfo();
            previousState = state;
            break;
    } 
    // Read Button States
    button1State = digitalRead(button1Pin);
    button2State = digitalRead(button2Pin);
    button3State = digitalRead(button3Pin);
    // Toggle Switch HIGH - ON, LOW - OFF
    button4State = digitalRead(button4Pin);
    // Handle Button Presses
    if (button1State == LOW || button2State == LOW || button3State == LOW)
    {
        // Button 1 - Red
        if (button1State == LOW) {
          // Cycle to the next mode
          switch (state)
          {
              case SHOW_TIME:
                  state = SHOW_SYSTEM_INFO;
                  Serial.println("Button 1 Pressed - Mode Changed to SHOW_SYSTEM_INFO");
                  break;
              case SHOW_SYSTEM_INFO:
                  lcd.setCursor(0, 0);
                  state = SHOW_CUSTOM_TEXT;
                  Serial.println("Button 1 Pressed - Mode Changed to SHOW_CUSTOM_TEXT");
                  break;
              case SHOW_CUSTOM_TEXT:
                  state = SHOW_SPLASH;
                  Serial.println("Button 1 Pressed - Mode Changed to SHOW_SPLASH");
                  break;
          }
          delay(500);
        }
        // Button 2 - Green
        if (button2State == LOW) { 
          Serial.println("Button 2 Pressed - Green");
          delay(150);
          play_rtttl(hamster_dance);
          Serial.println("Done Playing Songs.");
          delay(500);
        }
        // Button 3 - Blue
        if (button3State == LOW) { 
           Serial.println("Button 3 Pressed - Blue");
           play_rtttl(twentyfour_phone);
           delay(150);
            play_rtttl(nokia);
           delay(150);

        }
    }
    if (previousToggleState != button4State && (state == SHOW_TIME || state == SHOW_CUSTOM_TEXT)){
      if(button4State == HIGH){
        Serial.println("Toggle Switch Toggled - ON");
        lcd.setCursor(18, 1);
        lcd.print("AL");
      }
      else{
        Serial.println("Toggle Switch Toggled - Off");
        lcd.setCursor(18, 1);
        lcd.print("  ");
      }
      previousToggleState = button4State;
      delay(500);
    }
}

///////////////
// Functions //
///////////////

// show splash screen
void showSplash()
{  
  drawSideColumns(false);
  lcd.setCursor(3, 0);
  lcd.print("Garth's Clock"); 
  lcd.setCursor(3, 1);
  lcd.print("==============");
  lcd.setCursor(3, 2);
  lcd.print("Arduino DS1307");
  lcd.setCursor(3, 3);
  lcd.print(F("V")); 
  lcd.print(build); 
  lcd.print(F(".")); 
  lcd.print(revision);
  lcd.print(F(" RAM:"));  
  lcd.print(freeRam()); 
  lcd.print(F("B")); 
}
// show system information screen
void showSystemInfo()
{  
  drawSideColumns(true);
  lcd.setCursor(2, 0);
  lcd.print("Arduino RT Clock"); 
  lcd.setCursor(2, 1);
  lcd.print("3 btn, rocker sw");
  lcd.setCursor(2, 2);
  lcd.print("20X4 LCD, DS1307"); 
  lcd.setCursor(2, 3); 
  lcd.print(F("[V")); 
  lcd.print(build); 
  lcd.print(F(".")); 
  lcd.print(revision);
  lcd.print(F(" RAM: "));  
  lcd.print(freeRam()); 
  lcd.print(F("B]")); 
}
// show text received over a serial connection
void showSerialText()
{
  // Initial text only drawn the first time through 
  if(previousState != state)
  {
    lcd.setCursor(0, 0);
    lcd.print("Awaiting Serial Text"); 
  }
  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the top row
    lcd.setCursor(0,0); 
    lcd.print("                    ");
    lcd.setCursor(0,0); 
    String txtMsg;
    // add any incoming characters to the String:
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      txtMsg += inChar;
      if(txtMsg.length() < 21)
      {
        lcd.write(inChar);
      }
    } 
  }
}

// Displays the current date in the top row 
// e.g. SAT FEB 4,2019
void showDate()
{
   DateTime now = RTC.now();
   int startCol = 3;
   if(now.day() > 10){ startCol = 2; }
   lcd.setCursor(startCol, 0); 
   String dateString = String(dayName[now.dayOfTheWeek()]) + " " + String(monthName[now.month()-1]) + " " + String(now.day()) + "," + String(now.year());
   lcd.print(dateString);
}
// displays time in large 3 row letters
// starting on the second row
// 10:59 AM
void showTime() { 
  DateTime now = RTC.now();
  // Standard (AM/PM) or 24 Hour Clock Setup
  hr_24 = now.hour();
  if (hr_24 == 0) { // 12 AM
      hr_12 = 12;
      ampm = "AM";
  }
  else if(hr_24 == 12) { // 12 PM
      hr_12 = hr_24;
      ampm = "PM";
  }
  else if (hr_24 > 12) { // 13-23 PM
     hr_12 = (hr_24 - 12);
     ampm = "PM";
  }
  else { // 1-11 AM
    hr_12 = hr_24;
    ampm = "AM";
  }
  // Time Starting Point
  byte row = 1,col = 0;
  // Indent if not showing seconds
  if(!showSeconds){
    col = col + 4;
  }
  // Hours
  if (militaryTime && hr_24 < 10 ) {  
    col = col + 3;
    bigNumber(hr_24,row,col);
  }
  else if(!militaryTime && hr_12 < 10) {
    col = col + 3;
    bigNumber(hr_12,row,col);
  }
  else{
    if(militaryTime){
      bigNumber(getDigit(hr_24,2) ,row,col);
      col = col + 3;
      bigNumber(getDigit(hr_24,1) ,row,col);
    }
    else{
      bigNumber(getDigit(hr_12,2) ,row,col);
      col = col + 3;
      bigNumber(getDigit(hr_12,1) ,row,col);
    }
  }
  if(!showSeconds){
    col = col + 1;
  }  
  col = col + 2;
  bigNumber(11,row,col);
  col = col + 1;
  // Minutes
  if (now.minute() < 10) {  
    bigNumber(0,row,col);
    col = col + 3;
    bigNumber(now.minute(),row,col);
  }
  else {
    bigNumber(getDigit(now.minute(),2) ,row,col);
    col = col + 3;
    bigNumber(getDigit(now.minute(),1) ,row,col);
  }
  col = col + 2;
  // Seconds
  if(showSeconds){
   bigNumber(11,row,col);
   col = col + 1;
   if (now.second() < 10) {  
      bigNumber(0,row,col);
      col = col + 3;
      bigNumber(now.second(),row,col);
   }
   else {
     bigNumber(getDigit(now.second(),2) ,row,col);
     col = col + 3;
     bigNumber(getDigit(now.second(),1) ,row,col);
   }
  }
  // Time Type Indicator On Right
  if(militaryTime) {
    lcd.setCursor(18, 3);
    lcd.print("24");
  }
  else {
    lcd.setCursor(18, 3);
    lcd.print(ampm);
  }
  // Alarm Indicator On Right - Toggle Switch
  if(button4State == HIGH){
    lcd.setCursor(18, 1);
    lcd.print("AL");
  }
  else{
    lcd.setCursor(18, 1);
    lcd.print("  ");
  }
}
// draw a loading screen
void drawLoadingScreen()
{

  lcd.clear();
  //drawSolidBorder();
  lcd.setCursor(1, 1);
  lcd.print("Loading . . . . .");
  lcd.setCursor(1, 2);
  int count = 18;
  while(count > 0 )
  {
    lcd.print((char)255);
    delay(150);
    count = count -1;
  }
}
// draw columns on the side of screens
// single or double column
void drawSideColumns(bool singleColumn){
  // Left Column
  lcd.setCursor(0, 0);
  lcd.print((char)255);
  if(!singleColumn) { lcd.print((char)255); }
  lcd.setCursor(0, 1);
  lcd.print((char)255);
  if(!singleColumn) { lcd.print((char)255); }
  lcd.setCursor(0, 2);
  lcd.print((char)255);
  if(!singleColumn) { lcd.print((char)255); }
  lcd.setCursor(0, 3);
  lcd.print((char)255);
  if(!singleColumn) { lcd.print((char)255); }
  // Right Column
  lcd.setCursor(19, 0);
  lcd.print((char)255);
  if(!singleColumn) {
    lcd.setCursor(18, 0);
    lcd.print((char)255);
  }
  lcd.setCursor(19, 1);
  lcd.print((char)255);
  if(!singleColumn) { 
    lcd.setCursor(18, 1);   
    lcd.print((char)255);
  }
  lcd.setCursor(19, 2);
  lcd.print((char)255);
  if(!singleColumn) {
    lcd.setCursor(18, 2);
    lcd.print((char)255);
  }
  lcd.setCursor(19, 3);
  lcd.print((char)255);
  if(!singleColumn) {
    lcd.setCursor(18, 3);
  }
  lcd.print((char)255);  
}
// draw a solid border around the screen
void drawSolidBorder(){
  lcd.setCursor(0, 0);
  int topCount = 20;
  while(topCount > 0 )
  {
    lcd.print((char)255);
    topCount = topCount -1;
  }
  lcd.setCursor(0, 1);
  lcd.print((char)255);
  lcd.setCursor(19, 1);
  lcd.print((char)255);
  lcd.setCursor(0, 2);
  lcd.print((char)255);
  lcd.setCursor(19, 2);
  lcd.print((char)255);
  lcd.setCursor(0, 3);
  int bottomCount = 20;
  while(bottomCount > 0 )
  {
    lcd.print((char)255);
    bottomCount = bottomCount -1;
  }
  lcd.setCursor(1, 1);
}
// Position a custom big font digit
// position number 'num' starting top left at row 'r', column 'c'
void bigNumber(byte num, byte r, byte c) {
    lcd.setCursor(c,r);
    switch(num) {
      case 0: lcd.write(byte(2)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+1); lcd.write(byte(5)); lcd.write(byte(6)); 
              lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(3)); break;
            
      case 1: lcd.write(byte(0)); lcd.write(byte(1)); 
              lcd.setCursor(c,r+1); lcd.print(" "); lcd.write(byte(5));
              lcd.setCursor(c,r+2); lcd.write(byte(0)); lcd.write(byte(4)); break;
            
      case 2: lcd.write(byte(2)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+1); lcd.write(byte(2)); lcd.write(byte(3)); 
              lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(2)); break;  
            
      case 3: lcd.write(byte(2)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+1); lcd.write(byte(0)); lcd.write(byte(3)); 
              lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(3)); break;  
            
      case 4: lcd.write(byte(1)); lcd.write(byte(0)); 
              lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3)); 
              lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(6)); break;  
            
      case 5: lcd.write(byte(2)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+2); lcd.write(byte(2)); lcd.write(byte(3)); break;  
              
      case 6: lcd.write(byte(1)); lcd.print(" ");     
              lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(3)); break;  

      case 7: lcd.write(byte(2)); lcd.write(byte(2));
              lcd.setCursor(c,r+1); lcd.print(" "); lcd.write(byte(6)); 
              lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(6)); break;  

      case 8: lcd.write(byte(2)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3)); 
              lcd.setCursor(c,r+2); lcd.write(byte(4)); lcd.write(byte(3)); break;  
   
      case 9: lcd.write(byte(2)); lcd.write(byte(2)); 
              lcd.setCursor(c,r+1); lcd.write(byte(4)); lcd.write(byte(3)); 
              lcd.setCursor(c,r+2); lcd.print(" "); lcd.write(byte(6)); break;  

       case 11: lcd.setCursor(c,r+1); lcd.write(byte(7)); lcd.setCursor(c,r+2); lcd.write(byte(7)); break; 
    } 
}
// Count the number of digits in a number
// used to draw the individual big numbers
byte countDigits(int num){
  byte count=0;
  while(num){
    num=num/10;
    count++;
  }
  return count;
}
// Get a digit of a number 
// used to draw the individual big numbers one at a time
int getDigit(unsigned int number, int digit) {
    for (int i=0; i<digit-1; i++) { 
      number /= 10; 
    }
    return number % 10;
}
// FREERAM: Returns the number of bytes currently free in RAM  
int freeRam(void) {
  extern int  __bss_end, *__brkval; 
  int free_memory; 
  if((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end); 
  } 
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval); 
  }
  return free_memory; 
}

/////////////////////
// Tones and Music //
/////////////////////
void play_rtttl(char *p)
{
  // Absolutely no error checking in here
  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  while(*p != ':') p++;    // ignore name
  p++;                     // skip ':'

  // get default duration
  if(*p == 'd')
  {
    p++; p++;              // skip "d="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if(num > 0) default_dur = num;
    p++;                   // skip comma
  }

  Serial.print("ddur: "); Serial.println(default_dur, 10);

  // get default octave
  if(*p == 'o')
  {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if(num >= 3 && num <=7) default_oct = num;
    p++;                   // skip comma
  }

  Serial.print("doct: "); Serial.println(default_oct, 10);

  // get BPM
  if(*p == 'b')
  {
    p++; p++;              // skip "b="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                   // skip colon
  }

  Serial.print("bpm: "); Serial.println(bpm, 10);

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

  Serial.print("wn: "); Serial.println(wholenote, 10);

  // now begin note loop
  while(*p)
  {
    // first, get note duration, if available
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    
    if(num) duration = wholenote / num;
    else duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

    // now get the note
    note = 0;

    switch(*p)
    {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if(*p == '#')
    {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if(*p == '.')
    {
      duration += duration/2;
      p++;
    }
  
    // now, get scale
    if(isdigit(*p))
    {
      scale = *p - '0';
      p++;
    }
    else
    {
      scale = default_oct;
    }

    scale += OCTAVE_OFFSET;

    if(*p == ',')
      p++;       // skip comma for next note (or we may be at the end)

    // now play the note
    if(note)
    {
      Serial.print("Playing: ");
      Serial.print(scale, 10); Serial.print(' ');
      Serial.print(note, 10); Serial.print(" (");
      Serial.print(notes[(scale - 4) * 12 + note], 10);
      Serial.print(") ");
      Serial.println(duration, 10);
      tone(tonePin, notes[(scale - 4) * 12 + note]);
      delay(duration);
      noTone(tonePin);
    }
    else
    {
      Serial.print("Pausing: ");
      Serial.println(duration, 10);
      delay(duration);
    }
  }
}
