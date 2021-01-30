#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#include "ESP8266WiFi.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) { // Address 0x3c for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  loadingScreen();
}

void loop() {
  Serial.println("scan start");
  scanningScreen();
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n); Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      networkDetails(i, n);
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      Serial.print("Security: "); Serial.println(encryptionTypeStr(WiFi.encryptionType(i)));
      delay(10);
    }
  }
  Serial.println("");
  // Wait a bit before scanning again
  delay(5000);
}

void loadingScreen(void) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("WiFi Scan"));
  display.setCursor(0, 20);
  display.println(F("  ESP8266"));
  display.setTextSize(1); // Draw 1X-scale text
  display.setCursor(0, 45);
  display.println(F("       Built By"));
  display.setCursor(0, 55);
  display.println(F(" Garth Vander Houwen"));
  display.display(); 
  delay(5000);
}

void scanningScreen(void) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.println(F("Scanning"));
    display.setCursor(0, 45);
  display.fillCircle(display.width() / 6, 40, 9, SSD1306_INVERSE);
  display.fillCircle(display.width() / 2, 40, 9, SSD1306_INVERSE);
  display.fillCircle((display.width() - display.width() / 6), 40, 9, SSD1306_INVERSE);
  display.display(); 
}

void networkDetails(int i, int n) {
  display.clearDisplay();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("SSID "); display.print(i + 1); display.print("/"); display.println(n);
  display.setCursor(0, 20);
  display.setTextSize(1); // Draw 2X-scale text
  display.println(WiFi.SSID(i));
  display.setCursor(0, 32);
  display.print("Security: "); display.println(encryptionTypeStr(WiFi.encryptionType(i)));
  display.setCursor(0, 44);
  int bars = getBarsSignal(WiFi.RSSI(i));
  display.print("Channel: "); display.println(WiFi.channel(i));
  display.setCursor(0, 56);
  display.print("RSSI: "); display.print(WiFi.RSSI(i)); display.println("dBm"); 
  for (int b=0; b <= bars; b++) {
    //display.fillRect(59 + (b*5),33 - (b*5),3,b*5,WHITE); 
    display.fillRect(104 + (b*5),64 - (b*5),3,b*5,WHITE); 
  }
  display.display(); 
  delay(2000);
}

String encryptionTypeStr(uint8_t authmode) {
    switch(authmode) {
        case ENC_TYPE_NONE:
            return "OPEN";
        case ENC_TYPE_WEP:
            return "WEP";
        case ENC_TYPE_TKIP:
            return "WPA_PSK";
        case ENC_TYPE_CCMP:
            return "WPA2_PSK";
        case ENC_TYPE_AUTO:
            return "AUTO";
        default:
            return "UNKOWN";
    }
}

int getBarsSignal(long rssi){
  // 5. High quality: 90% ~= -55db
  // 4. Good quality: 75% ~= -65db
  // 3. Medium quality: 50% ~= -75db
  // 2. Low quality: 30% ~= -85db
  // 1. Unusable quality: 8% ~= -96db
  // 0. No signal
  int bars;
  
  if (rssi > -55) { 
    bars = 5;
  } else if (rssi < -55 & rssi > -65) {
    bars = 4;
  } else if (rssi < -65 & rssi > -75) {
    bars = 3;
  } else if (rssi < -75 & rssi > -85) {
    bars = 2;
  } else if (rssi < -85 & rssi > -96) {
    bars = 1;
  } else {
    bars = 0;
  }
  return bars;
}