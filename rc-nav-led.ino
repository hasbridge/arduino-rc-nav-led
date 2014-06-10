/*
****************************************************
RC LED Navigation Lights

Fades in and out an LED to simulate a rotating anti-collision beacon
Blinks strobe LED(s)
 
@author Harold Asbridge
@version 0.1
@date 2014-06-07
****************************************************
*/
 
// Anti-collision LED pin number - needs to support PWM
int acLedPin = 3;

// Anti-collision LED minimum fade brightness (0-255)
int minFade = 5;

// Anti-collision LED maximum fade brightness (0-255)
int maxFade = 127;

// How long to wait between fade steps (milliseconds)
int fadeInterval = 50;

// Strobe LED pin number
int strobeLedPin = 4;

// Don't change, these are used to store the current state of the anti-collision LED
int currentFade = minFade + 1;
int dir = 1;

void setup() {
  // initialize our pins as output
  pinMode(acLedPin, OUTPUT);
  pinMode(strobeLedPin, OUTPUT);
}
  
void loop() {
  // Fade in anti-collision LED
  int i;
  for (i = minFade; i <= maxFade; i+= 5) {
    analogWrite(acLedPin, i);
    delay(fadeInterval);
  }
  
  // Blink anti-collision and strobe once
  digitalWrite(strobeLedPin, HIGH);
  analogWrite(acLedPin, 255);
  delay(100);
  
  // Turn off strobe
  digitalWrite(strobeLedPin, LOW);
  
  // Put anti-collision back at the max fade brightness
  analogWrite(acLedPin, maxFade);
  
  // Flash strobe again
  delay(50);
  digitalWrite(strobeLedPin, HIGH);
  delay(100);
  digitalWrite(strobeLedPin, LOW);
  
  // Fade out anti-collision LED
  for (i = maxFade; i >= minFade; i-= 5) {
    analogWrite(acLedPin, i);
    delay(fadeInterval); 
  }
}
