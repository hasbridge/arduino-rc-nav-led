/**
 * RC LED Light Controller
 * 
 * Designed for an ATTiny85, but should run on any arduino. Default pin numbers are for an ATTiny85
 * 
 * Features:
 * Landing light controlled via an RC channel (can use a Y-harness on flaps or landing gear channel)
 * 2 fading anti-collision beacons
 * Double flash strobe
 * 
 * @author Harold Asbridge
 * @version 0.3
 * @date 2014-06-12
 * 
 */

// Landing light settings
#define LL_IRQ_NUMBER 0 // Interrupt number to use (0 = pin 2 on most boards)
#define LL_PIN_SERVO 2 // Servo input pin number - this needs to match whatever interrupt is used
#define LL_PIN_LIGHT 3 // Landing light output pin number
#define LL_SERVO_THRESHOLD 1500 // Servo signal threshold to turn on landing light (pulse width in microseconds, 1000 to 2000)
#define LL_SERVO_DEAD_ZONE 100 // Servo signal threshold to turn off light (pulse width in microseconds, 1000 to 2000)
#define LL_SERVO_REVERSED true   // Uncomment this line if your servo channel is reversed

// Strobe settings
#define STB_PIN_LIGHT 4 // Pin number for strobe light output
#define STB_BLINK_INTERVAL 2000000 // Blink interval for strobe light in microseconds

// Anti-collision beacon settings
#define ACB1_PIN_LIGHT 0 // Pin number for anti-collision beacon 1
#define ACB2_PIN_LIGHT 1 // Pin number for anti-collision beacon 2
#define ACB_FADE_MIN 10 // Minimum fade level for beacon (0-255)
#define ACB_FADE_MAX 50 // Maximum fade level for beacon (0-255)
#define ACB_FADE_INTERVAL 30000 // Fade step interval, in microseconds (lower numbers = faster fade)


// Var declarations
volatile unsigned long servoPulseStartTime;
volatile int servoPulseWidth = 0;
boolean curLandingLight = false;

unsigned long lastFadeTime = 0;
unsigned long lastStrobeTime = 0;
int currentFade = ACB_FADE_MIN;
int fadeDirection = 1;

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
  unsigned long currentTime = micros();

  // Check if the landing light should be turned on
  checkLandingLight();
  
  // Check if it's time to fade the anti-collision lights
  if ((currentTime - lastFadeTime) > ACB_FADE_INTERVAL) {
    doFade();
    lastFadeTime = currentTime;
  }

  // Check if it's time to blink the strobes
  if ((currentTime - lastStrobeTime) > STB_BLINK_INTERVAL) {
    doStrobe();
    lastStrobeTime = currentTime; 
  }
}

// Check servo signal, and decide to turn on/off the landing light
void checkLandingLight()
{
  // Modify threshold to prevent flicker
  int threshold = LL_SERVO_THRESHOLD;
  if (!curLandingLight) {
    // Light is not on, adjust threshold up
    threshold += LL_SERVO_DEAD_ZONE;
  } else {
    // Light is on, adjust threshold down
    threshold -= LL_SERVO_DEAD_ZONE;
  }

  // Check servo position
  if (servoPulseWidth >= threshold) {
    setLandingLight(true);
  } else {
    setLandingLight(false);
  }
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

// Fade anti-collision LEDs
void doFade()
{
  currentFade += fadeDirection;
  if (currentFade == ACB_FADE_MAX || currentFade == ACB_FADE_MIN) {
    // If we hit the fade limit, flash the high beacon, and flip the fade direction
    if (fadeDirection == 1) {
      analogWrite(ACB1_PIN_LIGHT, 255);

    } else {
      analogWrite(ACB2_PIN_LIGHT, 255);
    }
    delay(50); 
    fadeDirection *= -1; 
  }

  analogWrite(ACB1_PIN_LIGHT, currentFade);
  analogWrite(ACB2_PIN_LIGHT, ACB_FADE_MAX - currentFade + ACB_FADE_MIN);
}

// Strobe double-blink
void doStrobe()
{
  digitalWrite(STB_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB_PIN_LIGHT, LOW);
  delay(50);
  digitalWrite(STB_PIN_LIGHT, HIGH);
  delay(50);
  digitalWrite(STB_PIN_LIGHT, LOW);
}

// Measure servo PWM signal
void measureServoSignal()
{
  int pinState = digitalRead(LL_PIN_SERVO);
  if(pinState == HIGH) { 
    // Beginning of PWM pulse, mark time
    servoPulseStartTime = micros();
  } else {
    // End of PWM pulse, calculate pulse duration in mcs
    servoPulseWidth = (int)(micros() - servoPulseStartTime);

    // If servo channel is reversed, use the inverse
    if (LL_SERVO_REVERSED) {
      servoPulseWidth = (1000 - (servoPulseWidth - 1000)) + 1000;
    }
  }
}
