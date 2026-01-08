#include <Arduino.h>

const int buttonPin {3}; 
const int ledPin {6}; 

unsigned long lastBlinkTime {}; 
unsigned long lastPressTime {}; 

int counter {-1}; 
bool lastButtonState {HIGH}; 

void setup() {
  pinMode(ledPin, OUTPUT); 
  pinMode(buttonPin, INPUT_PULLUP); 
}


void loop() {
  bool currentButtonState {digitalRead(buttonPin)}; 

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    unsigned long currentPress {millis()}; 

    if (currentPress - lastPressTime >= 200) {
      counter = (counter + 1) % 3;
      lastPressTime = currentPress; 
    }

  }
 
  lastButtonState = currentButtonState; 
 
  switch (counter) { 
    case 0: 
      if (millis() - lastBlinkTime >= 500) {
        digitalWrite(ledPin, !digitalRead(ledPin)); 
        lastBlinkTime = millis(); 
      }
      break; 
    case 1: 
      digitalWrite(ledPin, HIGH); 
      break; 
    case 2: 
      digitalWrite(ledPin, LOW); 
      break; 
  }
}
