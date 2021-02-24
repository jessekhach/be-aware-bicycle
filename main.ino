#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>

// Defining where each pin is connected for ultrasonic sensors
const int frontLeftTrigger = 2;
const int frontLeftEcho = 3;
const int backLeftTrigger = 4;
const int backLeftEcho = 5;
const int frontRightTrigger = 6;
const int frontRightEcho = 7;
const int backRightTrigger = 8;
const int backRightEcho = 9;
const int frontTrigger = 10;
const int frontEcho = 11;

// Defining transistor pins to toggle vibrations
const int leftHandlebar = 13;
const int rightHandlebar = 22;
const int seat = 23;

// Defining the LED strip parameters
#define LED_PIN     12
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600); // For outputting distance data
  
  // Defining all pins as inputs or outputs
  pinMode(frontLeftEcho,INPUT_PULLUP); 
  pinMode(frontLeftTrigger, OUTPUT); 
  pinMode(backLeftEcho,INPUT_PULLUP);  
  pinMode(backLeftTrigger, OUTPUT);  
  pinMode(frontRightEcho,INPUT_PULLUP);  
  pinMode(frontRightTrigger, OUTPUT);  
  pinMode(backRightEcho,INPUT_PULLUP);  
  pinMode(backRightTrigger, OUTPUT);  
  pinMode(frontEcho,INPUT_PULLUP);  
  pinMode(frontTrigger, OUTPUT);  
  
  // Defining the digital trigger pins
  digitalWrite(frontLeftTrigger, LOW);
  digitalWrite(backLeftTrigger, LOW);
  digitalWrite(frontRightTrigger, LOW);
  digitalWrite(backRightTrigger, LOW);
  digitalWrite(frontTrigger, LOW);
  
  // Setting up the LED strip
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10 );
  delay(2000);
}

void loop()
{
  // Defining a default distance in the scope
  int frontLeftDist = 0;
  int backLeftDist = 0;
  int frontRightDist = 0;
  int backRightDist = 0;
  int frontDist = 0;
  
  // Achieving the distances from each sensor in cm
  frontLeftDist = getDistance(frontLeftTrigger, frontLeftEcho);
  backLeftDist = getDistance(backLeftTrigger, backLeftEcho);
  frontRightDist = getDistance(frontRightTrigger, frontRightEcho);
  backRightDist = getDistance(backRightTrigger, backRightEcho);
  frontDist = getDistance(frontTrigger, frontEcho);
  
  // Debugging function, unsure how to fix yet
  copycat();

  // Determine whether LEDs should be enabled
  checkLEDs(frontLeftDist, frontRightDist, backLeftDist, backRightDist, frontDist);
  
  // Determine whether front vibration motors should be activated
  checkFront(frontLeftDist, frontRightDist, frontDist);

  // Determine whether back vibration motor should be enabled
  checkBack(backLeftDist, backRightDist);
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
  // Set all LEDs to be white
  for (int i = 0; i <= 20; i++) {
    leds[i] = CRGB ( 255, 255, 255);
    FastLED.show();

  }
  delay(20);
  
  // Set all LEDs to be red
  for (int i = 20; i >= 0; i--) {
    leds[i] = CRGB ( 255, 0, 0);
    FastLED.show();

  }
  delay(20);
}

void stopLEDs()
{
  // Set all LEDs to be black or off
  for (int i = 0; i <= 20; i++) {
  leds[i] = CRGB ( 0, 0, 0);
  FastLED.show();

  }
}

void copycat()
{
  // Debugging function (doesn't actually output anything)
  for (int i = 0; i <= 20; i++) {
    leds[i] = CRGB ( 255, 0, 0);
  }
  delay(20);
  for (int i = 20; i >= 0; i--) {
    leds[i] = CRGB ( 0, 0, 0);
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
  // Determine whether the LEDs should turn off if the distance on any sensor is less than 60cm
  if (frontLeftDist < 60 || frontRightDist < 60 || backLeftDist < 60 || backRightDist < 60 || frontDist < 60)
  {
    startLEDs();
  }

  else
  {
    stopLEDs();
  }
}

void checkFront(int frontLeftDist, int frontRightDist, int frontDist)
{
  // Determine whether either front vibration motor should be on
  if ((frontLeftDist < 30 && frontRightDist < 30) || frontDist < 30) 
  {
    Serial.print("Starting full handlebar vibration\n");
    startVibrate(leftHandlebar);
    startVibrate(rightHandlebar);
  }

  else if (frontLeftDist < 30)
  {
    Serial.print("Starting left handlebar vibration\n");
    stopVibrate(rightHandlebar);
    startVibrate(leftHandlebar);
  }

  else if (frontRightDist < 30)
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
  // Determine whether the seat vibration motor should be on
  if (backLeftDist < 30 || backRightDist < 30)
  {
    Serial.print("Starting seat vibration\n");
    startVibrate(seat);
  }

  else
  {
    stopVibrate(seat);
  }
}
