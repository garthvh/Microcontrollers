#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define AVG_CHAR_PIXELS 6 // The display() method on the adafruit screen does not return the cursor position like the M5 stack screen library so I estimated.
#define TEXT_HEIGHT 8 // Height of text to be printed
#define TEXT_SIZE 1 //1 and 2 are available

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// I2C Adress of the CardKB
#define CARDKB_ADDR 0x5F

// I2C Adress of the OLED
#define OLED_ADDR 0x3c

// screen buffer for 9 rows of 21 characters max.
#define TEXT_BUFFER_HEIGHT_MAX 10
#define TEXT_BUFFER_LINE_LENGTH_MAX 22

char text_buffer[TEXT_BUFFER_HEIGHT_MAX][TEXT_BUFFER_LINE_LENGTH_MAX];
int text_buffer_height;
int text_buffer_line_length;
int text_buffer_write_pointer_x;
int text_buffer_write_pointer_y;
int text_buffer_read_pointer;

#define SCREEN_YSTARTPOS 56
#define SCREEN_XSTARTPOS 0
int screen_xpos = SCREEN_XSTARTPOS;

// start writing at the last line
int screen_ypos;
// maximum width of the screen
int screen_max;

// Enable or disable Waord Wrap
boolean tb_display_word_wrap = true;

void setup() {
  Wire.begin(D2, D1);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("=======================");
  Serial.println("|  CardKB Textbuffer  |");
  Serial.println("|  CardKB I2C : 0x5F  |");
  Serial.println("|    OLED I2C : 0x3c  |");
  Serial.println("|   03.02.2021 v1.0   |");
  Serial.println("=======================");
  tb_display_init();
}

void loop() {
  
  // check for serial input and print the received characters
  while(Serial.available() > 0){
    char data = Serial.read();
    tb_display_print_char(data);
  }

  // check for input from the Keyboard Hat and print the received characters
  Wire.requestFrom(CARDKB_ADDR, 1);
  while (Wire.available())
  {
    char c = Wire.read(); // receive a byte as characterif
    if (c != 0)
    { 
      if (c == 13) { //0x0D = CR = '\r'
        // Map CR to LF (0x0A)
        tb_display_print_char('\n');
        Serial.write('\n');
      } else {
        byte b = c;
        tb_display_print_char(c);
        Serial.write(c);
      }
      display.display();
    }
  }
}

// =============================================================
// Initialization of the Text Buffer and Screen
// Display size of SSD1306 = 128x64pixel
// With TEXT_HEIGHT=8, the screen can display 7 rows
// =============================================================
void tb_display_init(){
  
  // Start up the OLED Screen
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(TEXT_SIZE);
  display.setTextColor(SSD1306_WHITE);
  //display.cp437(true); 
  display.display();

  // 7 rows of text
  text_buffer_height = 8;
  text_buffer_line_length = TEXT_BUFFER_LINE_LENGTH_MAX;
  screen_max = SCREEN_WIDTH; 
  
  tb_display_clear();
  tb_display_show();
}

// =============================================================
// clear the text buffer
// without refreshing the screen
// call tb_display_show(); to clear the screen
// =============================================================
void tb_display_clear(){
  for(int line=0; line<TEXT_BUFFER_HEIGHT_MAX; line++){
    for(int charpos=0; charpos<TEXT_BUFFER_LINE_LENGTH_MAX; charpos++){
      text_buffer[line][charpos]='\0';
    }
  }
  text_buffer_read_pointer = 0;
  text_buffer_write_pointer_x = 0;
  text_buffer_write_pointer_y = text_buffer_height-1;
  screen_xpos = SCREEN_XSTARTPOS;
  screen_ypos = TEXT_HEIGHT*(text_buffer_height-1) -20;
}

// =============================================================
// clear the screen and display the text buffer
// =============================================================
void tb_display_show(){
  display.clearDisplay();
  int yPos = 0;
  for(int n=0; n<text_buffer_height; n++){
    // modulo operation for line position
    int line = (text_buffer_read_pointer+n) % text_buffer_height;
    int xPos = SCREEN_XSTARTPOS;
    int charpos=0;
    while(xPos < screen_max && text_buffer[line][charpos] != '\0'){
     display.setCursor(xPos, yPos);
     display.print(text_buffer[line][charpos]);
     xPos = xPos + AVG_CHAR_PIXELS;
     charpos++;
    }
    yPos = yPos + TEXT_HEIGHT;
  }
  screen_ypos = SCREEN_YSTARTPOS;
  screen_xpos = SCREEN_XSTARTPOS;
  display.display();
}

// =============================================================
// creates a new line and scroll the display upwards
// =============================================================
void tb_display_new_line(){
  text_buffer_write_pointer_x = 0;
  text_buffer_write_pointer_y++;
  text_buffer_read_pointer++;
  // circular buffer...
  if(text_buffer_write_pointer_y >= text_buffer_height)
    text_buffer_write_pointer_y = 0;
  if(text_buffer_read_pointer >= text_buffer_height)
    text_buffer_read_pointer = 0;
  // clear the actual new line for writing (first character a null terminator)
  text_buffer[text_buffer_write_pointer_y][text_buffer_write_pointer_x] = '\0';
  tb_display_show();
}

// =============================================================
// print a single character
// the character is added to the text buffer and
// directly printed via serial
// The text is automatically wrapped if longer than the display
// example: 
//    tb_display_print_char('X');
// =============================================================
void tb_display_print_char(char data){
  // check for LF for new line
  if (data == '\n') {
    // last character in the text_buffer line  should be always a null terminator
    text_buffer[text_buffer_write_pointer_y][text_buffer_write_pointer_x] = '\0';
    tb_display_new_line();
  }
  // only 'printable' characters
  if (data > 31 && data < 128) {
    // print the character and get the new xpos
    display.setCursor(screen_xpos, screen_ypos);
    display.print(data);
    screen_xpos += AVG_CHAR_PIXELS; 
    // if maximum number of characters reached
    if(text_buffer_write_pointer_x >= text_buffer_line_length-1){
      tb_display_new_line();
      // draw the character again because it was out of the screen last time
      display.setCursor(screen_xpos, screen_ypos);
      display.print(data);
      screen_xpos += AVG_CHAR_PIXELS;
    }
    // or if line wrap is reached
    if(screen_xpos >= screen_max) {
      // prepare for Word-Wrap stuff...
      // the buffer for storing the last word content
      char Char_buffer[TEXT_BUFFER_LINE_LENGTH_MAX];
      int n = 1;
      Char_buffer[0] = data;
      Char_buffer[n] = '\0';
      // if Word-Wrap, go backwards and get the last "word" by finding the
      // last space character:
      if(tb_display_word_wrap){
        int test_pos = text_buffer_write_pointer_x-1;
        // get backwards and search a space character
        while(test_pos > 0 && text_buffer[text_buffer_write_pointer_y][test_pos] != ' '){
          // store all the characters on the way back to the last space character
          Char_buffer[n] = text_buffer[text_buffer_write_pointer_y][test_pos];
          test_pos--;
          n++;
          Char_buffer[n] = '\0';
        }
        // if there was no space character in the row, Word-Wrap is not possible
        if(test_pos == 0) {
          // don't use the buffer but draw the character passed to the function
          n = 1;
        } else {
          // otherwise use the buffer to print the last found characters of the word
          // but only, if the charachter that causes a word wrap is not a space character
          if(data != ' '){
            // place a \0 at the position of the found space so that the drawing fuction ends here
            text_buffer[text_buffer_write_pointer_y][test_pos] = '\0';
          }
        }
      }
      tb_display_new_line();
      // icharacter passed to the function is a space character, then don't display
      // it as the first character of the new line
      if(data == ' ') 
        // don't use the buffer at all
        n = 0;
      n--;
      while(n >= 0){
        // draw the characters from the buffer back on the screen
        display.setCursor(screen_xpos, screen_ypos);
        display.println(Char_buffer[n]);
        screen_xpos += AVG_CHAR_PIXELS;
        // write the characters into the screen buffer of the new line
        text_buffer[text_buffer_write_pointer_y][text_buffer_write_pointer_x] = Char_buffer[n];
        text_buffer_write_pointer_x++;
        n--;
      }
      text_buffer[text_buffer_write_pointer_y][text_buffer_write_pointer_x] = '\0';
    } else {
      // write the character into the screen buffer
      text_buffer[text_buffer_write_pointer_y][text_buffer_write_pointer_x] = data;
      text_buffer_write_pointer_x++;
      // following character a null terminator to clear the old characters of the line
      text_buffer[text_buffer_write_pointer_y][text_buffer_write_pointer_x] = '\0';
    }
  }
}

// =============================================================
// print a string
// The string is added to the text buffer and directly printed
// on the screen.
// The otional parameter "chr_delay" allows a "character by character"
// processing of the String. Then, it looks like Teletype or Typewriter
// The delay is in milliseconds.
// The text is automatically wrapped if longer than the display
// example: 
//    tb_display_print_String("a new line\n");
//    tb_display_print_String("one\nand two lines\n");
//
//    char String_buffer[128]; 
//    snprintf(String_buffer, sizeof(String_buffer), "\nthe value: %i",value);
//    tb_display_print_String(String_buffer);
//
//    std::string msg;
//    msg = ss.str();
//    const char * c_msg = msg.c_str();
//    tb_display_print_String(c_msg);
// =============================================================
void tb_display_print_String(const char *s, int chr_delay){
  while(*s != 0){
    tb_display_print_char(*s++);
    if(chr_delay > 0)
      delay(chr_delay);
  }
}