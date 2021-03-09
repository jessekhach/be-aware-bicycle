#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>

// Defining where each pin is connected for ultrasonic sensors
const int frontLeftTrigger = 50;
const int frontLeftEcho = 52;
const int backLeftTrigger = 49;
const int backLeftEcho = 48;
const int frontRightTrigger = 22;
const int frontRightEcho = 23;
const int backRightTrigger = 7;
const int backRightEcho = 8;
const int frontTrigger = 53;
const int frontEcho = 2;

// Defining transistor pins to toggle vibrations
const int leftHandlebar = 51;
const int rightHandlebar = 4;
const int seat = 12;

// Defining the LED strip parameters
#define LED_PIN     13
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];
static int count = 0;

void setup() {
  Serial.begin(9600);           // For outputting distance data
  pinMode(frontLeftEcho,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(frontLeftTrigger, OUTPUT);  // Make the triggerPin an OUTPUT
  pinMode(backLeftEcho,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(backLeftTrigger, OUTPUT);  // Make the triggerPin an OUTPUT
  pinMode(frontRightEcho,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(frontRightTrigger, OUTPUT);  // Make the triggerPin an OUTPUT
  pinMode(backRightEcho,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(backRightTrigger, OUTPUT);  // Make the triggerPin an OUTPUT
  pinMode(frontEcho,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(frontTrigger, OUTPUT);  // Make the triggerPin an OUTPUT
  digitalWrite(frontLeftTrigger, LOW);
  digitalWrite(backLeftTrigger, LOW);
  digitalWrite(frontRightTrigger, LOW);
  digitalWrite(backRightTrigger, LOW);
  digitalWrite(frontTrigger, LOW);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS); // Sets up the LED strip
  FastLED.setBrightness(10 );
  delay(2000);
  Serial.print("Finished setup");
}

void loop()
{
  int frontLeftDist = 0;
  int backLeftDist = 0;
  int frontRightDist = 0;
  int backRightDist = 0;
  int frontDist = 0;
  frontLeftDist = getDistance(frontLeftTrigger, frontLeftEcho);
  backLeftDist = getDistance(backLeftTrigger, backLeftEcho);
  frontRightDist = getDistance(frontRightTrigger, frontRightEcho);
  backRightDist = getDistance(backRightTrigger, backRightEcho);
  frontDist = getDistance(frontTrigger, frontEcho);
  
  Serial.print(frontLeftDist, "\n");
  Serial.print(backLeftDist, "\n");
  Serial.print(frontRightDist, "\n");
  Serial.print(backLeftDist, "\n");
  Serial.print(frontRightDist, "\n");
  
  // Set up a counter to check if the obstacle is persistently detected
  if (frontLeftDist < 120 || frontRightDist < 120 || backLeftDist < 120 || backRightDist < 120 || frontDist < 120)
  {
    count++;
  }
  else
  {
    count = 0;
  }
  
  // If the obstacle is there for about 1.0 - 1.5 seconds, initialize the alert system
  if (count > 15)
  {
    copycat();

    checkLEDs(frontLeftDist, frontRightDist, backLeftDist, backRightDist, frontDist);
    
    checkFront(frontLeftDist, frontRightDist, frontDist);
  
    checkBack(backLeftDist, backRightDist);

  }
  
  // Otherwise, disable the alert system
  else
  {
    stopVibrate(leftHandlebar);
    stopVibrate(rightHandlebar);
    stopVibrate(seat);
    stopLEDs();
  }

}
int getDistance(int trigger, int echo) 
{

  long duration = 0;
  int distance = 0;
  int watchloop = 0;

  // Clear the triggerPin by setting it LOW and wait for any pulses to expire:

  digitalWrite(trigger, LOW);      // setting state to low and
  delay(2);                           // waiting 2,000uS (or 686cm (>22ft) to eliminate "echo noise")

  // only grab values under 20ft/610cm (for some reason, 676 is a common return error for ∞ distance)

  while ( (distance == 0) || (distance > 610) ) {

    // Trigger the sensor by setting the triggerPin high for 10 microseconds:

    digitalWrite(trigger, HIGH);         // start sending the 40kHz wave...
    delayMicroseconds(20);                  // sending for 20uS
    digitalWrite(trigger, LOW);          // stop sending 40kHz wave

    // Read the echoPin. pulseIn() duration of when the wave-echo stops (in microseconds):

    duration = pulseIn(echo, HIGH);

    // Calculate the distance:

    distance = duration * 0.034 / 2;

    // Catch funky ∞ distance readings

    watchloop++;        
    if (watchloop > 20){      // If errant "676" readings 20 times
      distance = 610;         // set distance to 610cm (20ft) 
      break;                  // and break out of loop (not really needed if forced to 610)
    }
  }

  return (distance);
}

void startLEDs()
{
  for (int i = 0; i <= 20; i++) {
    leds[i] = CRGB ( 255, 255, 255);
    FastLED.show();

  }
  delay(20);
  for (int i = 20; i >= 0; i--) {
    leds[i] = CRGB ( 255, 0, 0);
    FastLED.show();

  }
  delay(20);
}

void stopLEDs()
{
  for (int i = 0; i <= 20; i++) {
  leds[i] = CRGB ( 0, 0, 0);
  FastLED.show();

  }
}

void copycat()
{
  for (int i = 0; i <= 20; i++) {
    leds[i] = CRGB ( 255, 0, 0);
    // FastLED.show();

  }
  delay(20);
  for (int i = 20; i >= 0; i--) {
    leds[i] = CRGB ( 0, 0, 0);
    // FastLED.show();

  }
  delay(20);
}

void startVibrate(int vibrationPin)
{
  digitalWrite(vibrationPin, HIGH);
}

void stopVibrate(int vibrationPin)
{
  digitalWrite(vibrationPin, LOW);
}

void checkLEDs(int frontLeftDist, int frontRightDist, int backLeftDist, int backRightDist, int frontDist)
{
  int count;
  if (frontLeftDist < 120 || frontRightDist < 120 || backLeftDist < 120 || backRightDist < 120 || frontDist < 120)
  {
    startLEDs();
  }

  else
  {
    count = 0;
    stopLEDs();
  }
}

void checkFront(int frontLeftDist, int frontRightDist, int frontDist)
{
  if ((frontLeftDist < 60 && frontRightDist < 60) || frontDist < 60) 
  {
    Serial.print("Starting full handlebar vibration\n");
    startVibrate(leftHandlebar);
    startVibrate(rightHandlebar);
  }

  else if (frontLeftDist < 60)
  {
    Serial.print("Starting left handlebar vibration\n");
    stopVibrate(rightHandlebar);
    startVibrate(leftHandlebar);
  }

  else if (frontRightDist < 60)
  {
    Serial.print("Starting right handlebar vibration\n");
    stopVibrate(leftHandlebar);
    startVibrate(rightHandlebar);
  }

  else
  {
    stopVibrate(leftHandlebar);
    stopVibrate(rightHandlebar);
  }
}

void checkBack(int backLeftDist, int backRightDist)
{
  if (backLeftDist < 60 || backRightDist < 60)
  {
    Serial.print("Starting seat vibration\n");
    startVibrate(seat);
  }

  else
  {
    stopVibrate(seat);
  }
}

void checkCounter(int count)
