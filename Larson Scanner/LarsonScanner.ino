/*
 * Arrays containing the pin numbers used in
 * each charlieplexed matrix. In this case, both
 * matrices are the same size, and this code will
 * not work properly if this is not the case.
 */
 
int charlie1[3] = { 9, 10, 11};
int numPins = sizeof(charlie1)/sizeof(int);
int delayAmt = 400;
 
// Basic LED control functions
void allOff(int pinArray[], int del) {
  /* 
   * Turn off all LEDs in the array
   * pinArray = Array of pins to turn off
   * del = Delay before next action
   */
  for(int pin = 0; pin < numPins; pin++) {
    pinMode(pinArray[pin], OUTPUT);
    digitalWrite(pinArray[pin], LOW);
  }
  delay(del);
}
 
void allOn(int pinArray[], int del) {
  /*
   * Turn on all LEDs in the array
   * pinArray = Array of pins to turn on
   * del = Delay before next action
   *
   * NOTE: The LEDs will be less bright than
   *       normal, as to turn all of them on,
   *       we must change the duty cycle to 
   *       (1/number of LEDs in the array), which
   *       essentially accidentally implements
   *       PWM on every LED in the array.
   *       There is probably a more clever way
   *       to do this, so if you know how to do
   *       so, feel free to let me know!
   */
  int timer = 0;
  unsigned long prevTime = millis();
  while( timer < del ) {
    for(int i = 0; i < numPins; i++) {
      for(int j = 0; j < numPins; j++) {
        if(j == i)
          continue;
        switchOn(pinArray, i, j);
      }
    }
    timer = millis() - prevTime;
  }
}
 
void switchOn(int pinArray[], int source, int sink) {
  /*
   * Switch on an LED. Each LED in the
   * charlieplexed matrix can be referred to
   * by selecting a permutation of two pins 
   * from the array. Any which aren't selected
   * are put into INPUT mode, to disable them
   *
   * pinArray = An array of pins to work on
   * source = Current source pin
   * sink = Current sink pin
   */
  // Set pin modes
  pinMode(pinArray[source], OUTPUT);
  pinMode(pinArray[sink], OUTPUT);
  for(int pin = 0; pin < numPins; pin++)
  {
    if(pin == source) {
      // Set source to HIGH
      digitalWrite(pinArray[pin], HIGH);
    } else if(pin == sink) {
      // Set sink to LOW
      digitalWrite(pinArray[pin], LOW);
    } else {
      // Set any others to INPUT
      pinMode(pinArray[pin], INPUT);
    }
  }
}
 
void fadeSingle(int pinArray[],
                int source, int sink,
                int dir, int del) {
  /*
   * Fade a single charlieplexed LED in or out.
   * pinArray[] = Array of PWM pin numbers
   * source = Index of the source pin in the array
   * sink = Index of the sink pin in the array
   * dir = Direction of fade. 0 = Down; 1 = Up.
   * del = Fade delay(microseconds)
   */
  // Set pin modes
  pinMode(pinArray[source], OUTPUT);
  pinMode(pinArray[sink], OUTPUT);
 
  // Set the sink pin to low straight away
  digitalWrite(pinArray[sink], LOW);
 
  // Find any other pins in the array, and disable them
  // by setting them to INPUT
  for(int pin = 0; pin < numPins; pin++) {
    if(pin != source && pin != sink) {
      pinMode(pinArray[pin], INPUT);
    }
  }
 
  // Now fade the source pin in or out
  if(dir > 0) {
    for(int fadeVal = 0; fadeVal < 256; fadeVal++) {
      analogWrite(pinArray[source], fadeVal);
      delayMicroseconds(del);
    }
  } else {
    for(int fadeVal = 255; fadeVal >= 0; fadeVal--) {
      analogWrite(pinArray[source], fadeVal);
      delayMicroseconds(del);
    }
  }
}
 
void setup() {
  // Set all pins to output-
  for(int i = 0; i < numPins; i++) {
    pinMode(charlie1[i], OUTPUT);
  }
}

 
void loop() {
 
  // Blink each array 5 times, 500ms between changes
 // blinkAll(charlie1, 5, 500);
  // Switch through all permutations
  //allPermsSwitch();
  // Fade through all permutation
  //allPermsFade();
  // LED 1
  fadeSingle(charlie1, 0, 2, 1, delayAmt);
  // LED 2
  fadeSingle(charlie1, 0, 1, 1, delayAmt);
  // LED 3 
  fadeSingle(charlie1, 1, 2, 1, delayAmt);
  //LED 4
  fadeSingle(charlie1, 1, 0, 0, delayAmt);
  // LED 5
  fadeSingle(charlie1, 2, 1, 1, delayAmt);
  // LED 6
  fadeSingle(charlie1, 2, 0, 1, delayAmt);
  
  // LED 5
  fadeSingle(charlie1, 2, 1, 1, delayAmt);
    //LED 4
  fadeSingle(charlie1, 1, 0, 0, delayAmt);
     // LED 3 
  fadeSingle(charlie1, 1, 2, 1, delayAmt);
    // LED 2
  fadeSingle(charlie1, 0, 1, 1, delayAmt);
}
 
// Functions for loop
void allPermsSwitch() {
  /* 
   * Loop through all permutations, switching
   * each on then off with a 1000ms delay
   */
  for(int i = 0; i < numPins; i++) {
    for(int j = 0; j < numPins; j++) {
      if(i == j) {
        continue;
      }
      switchOn(charlie1, i, j);
      delay(1000);
      allOff(charlie1, 0);
    }
  }
}
 
void allPermsFade() {
  /*
   * Loop through all permutations, fading
   * each in then out with a 2ms delay between
   * increments
   */
  for(int i = 0; i < numPins; i++) {
    for(int j = 0; j < numPins; j++) {
      if(i == j) {
        continue;
      }
      Serial.print(i);       // prints a label
  Serial.print(j); 
  Serial.println("\t"); 
      fadeSingle(charlie1, i, j, 1, 2000);
      fadeSingle(charlie1, i, j, 0, 2000);
    }
  }
}
 
void blinkAll(int pinArray[], int reps, int del) {
  /*
   * Blink all LEDs in an array
   * rep = Repetitions
   * del = Delay between state changes
   */
  while(reps > 0) {
    allOn(pinArray, del);
    allOff(pinArray, del);
    reps--;
  }
}
