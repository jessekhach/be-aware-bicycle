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

// // OLED Declaration screen size in pixels
// #define SCREEN_WIDTH 128
// #define SCREEN_HEIGHT 64

// Defining the LED strip parameters
#define LED_PIN     12
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];

// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
  // display.display();
  // if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for(;;);
  // }
  delay(2000);

  // display.clearDisplay();
  // display.setTextSize(2);
  // display.setTextColor(WHITE);
  // display.setCursor(0, 0);
  // display.display();
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
  // Serial.print(frontLeftDist);
  // Serial.print(backLeftDist);
  // Serial.print(frontRightDist);
  // Serial.print(backLeftDist);
  // Serial.print(frontRightDist);
  // printToDisplay("Back left: ", backRightDist);
  // printToDisplay("Front: ", frontDist);
  // display.display();
  // display.clearDisplay();
  // display.setCursor(0,0);
  copycat();

  checkLEDs(frontLeftDist, frontRightDist, backLeftDist, backRightDist, frontDist);
  
  checkFront(frontLeftDist, frontRightDist, frontDist);

  checkBack(backLeftDist, backRightDist);

// void printToSerial(int index, int distance)
// {
//   char format[16];
//   sprintf(format, "Distance %i: %i \n", index, distance);
//   Serial.print(format);
// }

// void printToDisplay(int index, int distance)
// {
//   char format[16];
//   sprintf(format, "Distance %i: %i cm", index, distance);
//   display.println(format);
// }
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
