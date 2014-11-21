/*************************************************** 
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();




#define EYE_BALL_CENTER B11100111 
#define EYE_BALL_LEFT_1 B10011111 
#define EYE_BALL_RIGHT_1 B11110011 


/**

  Here we define list of bitmaps to construct each eye position. 
  A function is declared for each position which can be chained to animate more complex movement.


  Actual 8x8 dot matrix is 
  matrix    - actual
   up           left
*/




static uint8_t PROGMEM
eyes_shut_1_bmp[] = {
    B00000000,
    B01111110,
    B11111111,
    B11100111,
    B11100111,
    B11111111,
    B01111110,
    B00000000,
},

eyes_shut_2_bmp[] = {
    B00000000,
    B00000000,
    B11111111,
    B11100111,
    B11100111,
    B11111111,
    B00000000,
    B00000000
},
eyes_shut_3_bmp[] = {
    B00000000,
    B00000000,
    B00000000,
    B11100111,
    B11100111,
    B00000000,
    B00000000,
    B00000000
},

eyes_shut_bmp[] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
},

eyes_center_small_bmp[] = { 
    B00111100,
    B01111110,
    B11111111,
    EYE_BALL_CENTER,
    EYE_BALL_CENTER,
    B11111111,
    B01111110,
    B00111100
  },     

eyes_center_right_bmp[] = { 
    B00111100,
    B01111110,
    B11111111,
    EYE_BALL_RIGHT_1,
    EYE_BALL_RIGHT_1,
    B11111111,
    B01111110,
    B00111100
  }, 

eyes_center_right_max_bmp[] = { 
    B00100100,
    B01100110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B01111110,
    B00111100
  }, 

  
eyes_center_left_bmp[] = { 
    B00111100,
    B01111110,
    B11111111,
    EYE_BALL_LEFT_1,
    EYE_BALL_LEFT_1,
    B11111111,
    B01111110,
    B00111100
  },

eyes_center_left_max_bmp[] = { 
    B00111100,
    B01111110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B01100110,
    B00100100
  },

eyes_center_large_bmp[] = { 
    B00111100,
    B01101110,
    B11000111,
    B10000011,
    B11000111,
    B11101111,
    B00111100,
    B01111110
  },
  
  
eyes_center_up_bmp[] = { 
    B00111100,
    B01111110,
    B11111111,
    B11001111,
    B11001111,
    B11111111,
    B01111110,
    B00111100
  }, 
  
  
eyes_down_right_bmp[] = { 
    B00111100,
    B01111110,
    B11111001,
    B11111001,
    B11111111,
    B11111111,
    B01111110,
    B00111100
  },
  
 eyes_down_left_bmp[] = { 
    B00111100,
    B01111110,
    B10111111,
    B10111111,
    B11111111,
    B11111111,
    B01111110,
    B00111100
  },
  
  
eyes_up_right_bmp[] = { 
    B00111100,
    B01111110,
    B10011111,
    B10011111,
    B11111111,
    B11111111,
    B01111110,
    B00111100
  },
    
eyes_up_left_bmp[] = { 
    B00111100,
    B01111110,
    B11111111,
    B11011111,
    B10001111,
    B11011111,
    B01111110,
    B00111100
  }; 
 
static uint8_t *u8p;

/****
  Emulate basic animation of a pair of eyes using 8x8 LED Matrix

  look_eyes(uint8_t *bitmap, int delay)
    - set a bitmap over both eyes with a given delay
  
  blink_eyes(uint8_t *bitmap) 
    - using series of canned bitmaps, show eyes blinking starting from *bitmap and returning to *bitmap that is provided
    
  look_center_small()
    - eyes center and squint   
    
  look_center_large()
    - eyes center wide   
    
  look_center_left()
    - look left then blink  
    
  look_center_right()
    - look right then blink
  
  
*/





void static look_eyes(uint8_t *bitmap, int d){
  u8p = bitmap;
  matrix.clear();
  matrix.drawBitmap(0, 0, bitmap, 8, 8, LED_GREEN);
  matrix.writeDisplay(); 
  delay(d);
  
}

void static blink_eyes(uint8_t *bitmap){
  Serial.println("Blink eyes:");
   look_eyes(eyes_shut_1_bmp, 50);
   look_eyes(eyes_shut_2_bmp, 10);
   look_eyes(eyes_shut_3_bmp, 10);
   look_eyes(eyes_shut_bmp, 50);
   look_eyes(eyes_shut_3_bmp, 10);
   look_eyes(eyes_shut_2_bmp, 20);
   look_eyes(eyes_shut_1_bmp, 50);
   look_eyes(bitmap, 2000);  
}

void static look_center_small(){
  look_eyes(eyes_center_small_bmp, 1020);
  blink_eyes(eyes_center_small_bmp);
  
}

void static look_center_right(){
  look_eyes(eyes_center_right_bmp, 2000);
  blink_eyes(eyes_center_right_bmp);
}

void static look_center_right_max(){
 look_eyes(eyes_center_right_max_bmp, 2000);
}


void static look_center_left(){
 look_eyes(eyes_center_left_bmp, 1000);
 blink_eyes(eyes_center_left_bmp);
 
}

void static look_center_left_ma(){
  look_eyes(eyes_center_left_max_bmp, 2000);
}

void static look_center_large(){
 look_eyes(eyes_center_large_bmp, 1000);
}



/**   */

void static square() {
   matrix.drawRect(0, 0, 8, 8, LED_ON); 
}


/********
  Pair of eyes using Adafruit LEDBackpack and GFX
******/
void setup() {
  Serial.begin(9600);
  Serial.println("8x8 LED Matrix Test");
  matrix.begin(0x70);  // pass in the address
}

void loop() {
  // loop a series of eye movements
  look_center_small(); 
  look_center_right();
  look_center_left();
  delay(100);
}


