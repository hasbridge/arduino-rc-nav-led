/**
RC LED Light Controller

Designed for an ATTiny85, but should run on any arduino. Default pin numbers are for an ATTiny85

Features:
* Landing light controlled via an RC channel (can use a Y-harness on flaps or landing gear channel)
* 2 fading anti-collision beacons
* Double flash strobe

@author Harold Asbridge
@version 0.2
@date 2014-06-11

*/

// Landing light settings
#define LL_IRQ_NUMBER 0 // Interrupt number to use (0 = pin 2 on most boards)
#define LL_PIN_SERVO 2 // Servo input pin number - this needs to match whatever interrupt is used
#define LL_PIN_LIGHT 3 // Landing light output pin number
#define LL_SERVO_THRESHOLD 20 // Percent (0 to 100) of servo position required to turn on landing light
#define LL_SERVO_REVERSED false   // Uncomment this line if your servo channel is reversed

// Strobe settings
#define STB_PIN_LIGHT 4 // Pin number for strobe light output

// Anti-collision beacon settings
#define ACB1_PIN_LIGHT 0 // Pin number for anti-collision beacon 1
#define ACB2_PIN_LIGHT 1 // Pin number for anti-collision beacon 2
#define ACB_FADE_MIN 5 // Minimum fade level for beacon (0-255)
#define ACB_FADE_MAX 50 // Maximum fade level for beacon (0-255)
#define ACB_FADE_SPEED 0.035 // Fade speed, higher = faster, dependent on clock speed

// Other
#define BLINK_TIME 50 // How long an LED is on for a blink (in ms)


// Var declarations
volatile unsigned long servoPulseStartTime;
volatile int servoPosition = 0;
volatile int lastServoReading;
boolean curLandingLight = false;

// Called on power on or reset
void setup()
{
  // Set up interrupt handler
  attachInterrupt(LL_IRQ_NUMBER, measureServoSignal, CHANGE);

  // Declare output pins
  pinMode(LL_PIN_LIGHT, OUTPUT);
  pinMode(STB_PIN_LIGHT, OUTPUT);
  pinMode(ACB1_PIN_LIGHT, OUTPUT);
  pinMode(ACB2_PIN_LIGHT, OUTPUT);
}

// Called continuouly
void loop()
{
  // Check servo position
  if (servoPosition >= LL_SERVO_THRESHOLD) {
    setLandingLight(!LL_SERVO_REVERSED);
  } else {
    setLandingLight(LL_SERVO_REVERSED);
  }
  
  // Fade anti-collision LEDs
  float i;
  for (i = ACB_FADE_MIN; i <= ACB_FADE_MAX; i += ACB_FADE_SPEED) {
    analogWrite(ACB1_PIN_LIGHT, i);
    analogWrite(ACB2_PIN_LIGHT, ACB_FADE_MAX - i + ACB_FADE_MIN);
    //delay(ACB_FADE_DELAY);
  }
  
  // Blink anti-collision 1 and strobe once
  digitalWrite(STB_PIN_LIGHT, HIGH);
  digitalWrite(ACB1_PIN_LIGHT, HIGH);
  delay(BLINK_TIME);
  
  // Turn off strobe
  digitalWrite(STB_PIN_LIGHT, LOW);
  
  // Put anti-collision back at the max fade brightness
  analogWrite(ACB1_PIN_LIGHT, ACB_FADE_MAX);
  
  // Flash strobe again
  delay(BLINK_TIME);
  digitalWrite(STB_PIN_LIGHT, HIGH);
  delay(BLINK_TIME);
  digitalWrite(STB_PIN_LIGHT, LOW);
  
  // Fade anti-collision LEDs
  for (i = ACB_FADE_MAX; i >= ACB_FADE_MIN; i -= ACB_FADE_SPEED) {
    analogWrite(ACB1_PIN_LIGHT, i);
    analogWrite(ACB2_PIN_LIGHT, ACB_FADE_MAX - i + ACB_FADE_MIN);
    //delay(ACB_FADE_DELAY); 
  }
  
  // Blink anti-collision 2
  digitalWrite(ACB2_PIN_LIGHT, HIGH);
  delay(BLINK_TIME);
  analogWrite(ACB2_PIN_LIGHT, ACB_FADE_MAX);
  
}

// Turn on or off landing light
void setLandingLight(boolean state)
{
  float i;
  if (state && !curLandingLight) {
    digitalWrite(LL_PIN_LIGHT, HIGH);
  } else if (!state && curLandingLight) {
    digitalWrite(LL_PIN_LIGHT, LOW);
  }
  curLandingLight = state;
}

// Measure servo PWM signal
void measureServoSignal()
{
  int pinState = digitalRead(LL_PIN_SERVO);
  if(pinState == HIGH) { 
    // Beginning of PWM pulse, mark time
    servoPulseStartTime = micros();
  } else if (pinState == LOW) {
    // End of PWM pulse, calculate pulse duration in mcs
    int reading = (int)(micros() - servoPulseStartTime);
     
    // Average readings to filter out noise
    int average = (int)(reading + lastServoReading) / 2;
    lastServoReading = reading;
    
    // Convert to percentage
    servoPosition = (int)((average - 1000) / 10);
  }
}
