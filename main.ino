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


const int timeout = 15000;
const int dist = 200;

// Defining the LED strip parameters
#define LED_PIN     13
#define NUM_LEDS    10
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
  
  frontLeftDist = getDistance(frontLeftTrigger, frontLeftEcho, timeout);
  backLeftDist = getDistance(backLeftTrigger, backLeftEcho, timeout);
  frontRightDist = getDistance(frontRightTrigger, frontRightEcho, timeout);
  backRightDist = getDistance(backRightTrigger, backRightEcho, timeout);
  frontDist = getDistance(frontTrigger, frontEcho, timeout);
  
  Serial.print(frontLeftDist, "\n");
  Serial.print(backLeftDist, "\n");
  Serial.print(frontRightDist, "\n");
  Serial.print(backLeftDist, "\n");
  Serial.print(frontRightDist, "\n");
  
  checkFront(frontLeftDist, frontRightDist, frontDist);
  checkBack(backLeftDist, backRightDist);

}
int getDistance(int trigger, int echo, int timeout) 
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

    duration = pulseIn(echo, HIGH, timeout);

    // Calculate the distance:

    distance = duration * 0.034 / 2;

    // Catch funky ∞ distance readings

    watchloop++;        
    if (watchloop > 20){      // If errant "676" readings 20 times
      distance = 610;         // set distance to 610cm (20ft) 
      break;                  // and break out of loop (not really needed if forced to 610)
    }
  }
  Serial.print(distance, "\n");

  return (distance);
}

void stopLEDs()
{
  for (int i = 0; i <= 5; i++) {
  leds[i] = CRGB ( 0, 0, 0);
  FastLED.show();

  }
}

void startVibrate(int vibrationPin)
{
  digitalWrite(vibrationPin, HIGH);
}

void stopVibrate(int vibrationPin)
{
  digitalWrite(vibrationPin, LOW);
}

void checkFront(int frontLeftDist, int frontRightDist, int frontDist)
{
  if ((frontLeftDist < dist && frontRightDist < dist) || frontDist < dist) 
  {
    Serial.print("Starting full handlebar vibration\n");
    startVibrate(leftHandlebar);
    startVibrate(rightHandlebar);
    for (int i = 4; i <= 5; i++) {
    leds[i] = CRGB ( 0, 0, 255);
    FastLED.show();
    }
    for (int i = 0; i <= 1; i++) {
    leds[i] = CRGB ( 0, 255, 0);
    FastLED.show();
    }
    }
    
  else if (frontLeftDist < dist)
  {
    Serial.print("Starting left handlebar vibration\n");
    stopVibrate(rightHandlebar);
    for (int i = 4; i <= 5; i++) {
    leds[i] = CRGB ( 0, 0, 0);
    FastLED.show();
    }
    startVibrate(leftHandlebar);
    for (int i = 0; i <= 1; i++) {
    leds[i] = CRGB ( 0, 255, 0);
    FastLED.show();
    }
    }

  else if (frontRightDist < dist)
  {
    Serial.print("Starting right handlebar vibration\n");
    stopVibrate(leftHandlebar);
    for (int i = 0; i <= 1; i++) {
    leds[i] = CRGB ( 0, 0, 0);
    FastLED.show();
    }
    startVibrate(rightHandlebar);
    for (int i = 4; i <= 5; i++) {
    leds[i] = CRGB ( 0, 0, 255);
    FastLED.show();
    }
  }

  else
  {
    stopVibrate(leftHandlebar);
    stopVibrate(rightHandlebar);
    for (int i = 4; i <= 5; i++) {
    leds[i] = CRGB ( 0, 0, 0);
    FastLED.show();
    }
    for (int i = 0; i <= 1; i++) {
    leds[i] = CRGB ( 0, 0, 0);
    FastLED.show();
    }
}
}

void checkBack(int backLeftDist, int backRightDist)
{
  if (backLeftDist < dist || backRightDist < dist)
  {
    Serial.print("Starting seat vibration\n");
    startVibrate(seat);
    for (int i = 2; i <= 3; i++) {
    leds[i] = CRGB ( 255, 0, 0);
    FastLED.show();
    }

  }

  else
  {
    stopVibrate(seat);
    for (int i = 2; i <= 3; i++) {
    leds[i] = CRGB ( 0, 0, 0);
    FastLED.show();
  }
}
}
