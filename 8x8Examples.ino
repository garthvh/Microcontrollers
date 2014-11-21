/*************************************************** 
  Assorted Examples for use with the Adafruit HT16K33 
  Breakout board connected to an 8x8 LED matrix
 ****************************************************/

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void setup() {
  matrix.begin(0x70);  // pass in the address
}

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 },
  face1_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111}, // Each row here represents one frame of creeper face
  face2_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face3_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face4_bmp[] = { B11111111,B11111111,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face5_bmp[] = { B11111111,B11111111,B11111111,B11111111,B11100111,B11000011,B11011011,B11111111},
  face6_bmp[] = { B11111111,B11111111,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face7_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face8_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face9_bmp[] = { B11111111,B11111111,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face10_bmp[] = { B11111111,B11111111,B11111111,B11111111,B11100111,B11000011,B11011011,B11111111},
  face11_bmp[] = { B11111111,B11111111,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face12_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face13_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11100111,B11000011,B11011011,B11111111},
  face14_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11011011,B11011011,B11000011,B11111111},
  face15_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11011011,B11011011,B11000011,B11111111},
  face16_bmp[] = { B11111111,B10011001,B10011001,B11111111,B11011011,B11011011,B11000011,B11111111},
  alien1_bmp1[] =
  { B00011000, // This is the first frame for alien #1
    B00111100, // If you squint you can kind of see the
    B01111110, // image in the 0's and 1's.
    B11011011,
    B11111111,
    B00100100,
    B01011010,
    B10100101 },
    alien1_bmp2[] =
  { B00011000, // This is the second frame for alien #1
    B00111100,
    B01111110,
    B11011011,
    B11111111,
    B00100100,
    B01011010,
    B01000010 },
  alien2_bmp1[] =
  { B00000000, // First frame for alien #2
    B00111100,
    B01111110,
    B11011011,
    B11011011,
    B01111110,
    B00100100,
    B11000011 },
    alien2_bmp2[] =
  { B00111100, // Second frame for alien #2
    B01111110,
    B11011011,
    B11011011,
    B01111110,
    B00100100,
    B00100100,
    B00100100 },
    alien3_bmp1[] =
  { B00100100, // First frame for alien #3
    B00100100,
    B01111110,
    B11011011,
    B11111111,
    B11111111,
    B10100101,
    B00100100 },
    alien3_bmp2[] =
  { B00100100, // Second frame for alien #3
    B10100101,
    B11111111,
    B11011011,
    B11111111,
    B01111110,
    B00100100,
    B01000010 },
  alien4_bmp1[] =
  { B00111100, // First frame for alien #4
    B01111110,
    B00110011,
    B01111110,
    B00111100,
    B00000000,
    B00001000,
    B00000000 },
  alien4_bmp2[] =
  { B00111100, // Second frame for alien #4
    B01111110,
    B10011001,
    B01111110,
    B00111100,
    B00000000,
    B00001000,
    B00001000, },
  alien4_bmp3[] =
  { B00111100, // Third frame for alien #4 (NOT a repeat of frame 1)
    B01111110,
    B11001100,
    B01111110,
    B00111100,
    B00000000,
    B00000000,
    B00001000 },
  alien4_bmp4[] =
  { B00111100, // Fourth frame for alien #4 (NOT a repeat of frame 2)
    B01111110,
    B01100110,
    B01111110,
    B00111100,
    B00000000,
    B00000000,
    B00000000 };

void loop() {
  /* Alien #1 */
  matrix.clear();
  matrix.drawBitmap(0, 0, alien1_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien1_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien1_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien1_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(2000);
  
  /* Alien #2 */
  matrix.clear();
  matrix.drawBitmap(0, 0, alien2_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien2_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien2_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien2_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(2000);
  
  /* Alien #3 */
  matrix.clear();
  matrix.drawBitmap(0, 0, alien3_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien3_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien3_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien3_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(2000);
  
  /* Alien #4 */
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp3, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp4, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp1, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp2, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp3, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, alien4_bmp4, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(2000);
 
 /* Faces */
  matrix.clear();
  matrix.drawBitmap(0, 0, face1_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face2_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face3_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face4_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face5_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face6_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face7_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face8_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face9_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face10_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face11_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face12_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face13_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face14_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face15_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, face16_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(1500);
 
  /* Smilly Face */
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);

  matrix.clear();      // clear display
  matrix.drawPixel(0, 0, LED_GREEN);  
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawLine(0,0, 7,7, LED_GREEN);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawRect(0,0, 8,8, LED_GREEN);
  matrix.fillRect(2,2, 4,4, LED_GREEN);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.clear();
  matrix.drawCircle(3,3, 3, LED_GREEN);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);

  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextSize(1);
  matrix.setTextColor(LED_GREEN);
  for (int8_t x=7; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("HELLO");
    matrix.writeDisplay();
    delay(100);
  }
  /**matrix.setRotation(3); **/
  matrix.setTextColor(LED_GREEN);
  for (int8_t x=15,m; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("ELSIE");
    matrix.writeDisplay();
    delay(100);
  }
  matrix.setRotation(0);
}
