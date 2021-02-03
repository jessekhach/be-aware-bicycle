#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>

const int triggerPin = 2;
const int echoPin = 3;
const int triggerPin2 = 4;
const int echoPin2 = 5;
const int triggerPin3 = 6;
const int echoPin3 = 7;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define LED_PIN     8
#define NUM_LEDS    60
CRGB leds[NUM_LEDS];

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);           // For outputting distance data
  pinMode(echoPin,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(triggerPin, OUTPUT);  // Make the triggerPin an OUTPUT
  pinMode(echoPin2,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(triggerPin2, OUTPUT);  // Make the triggerPin an OUTPUT
  pinMode(echoPin3,INPUT_PULLUP);  // Someone said this also helped with errant readings...
  pinMode(triggerPin3, OUTPUT);  // Make the triggerPin an OUTPUT
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  digitalWrite(triggerPin, LOW);
  digitalWrite(triggerPin2, LOW);
  digitalWrite(triggerPin3, LOW);
  display.display();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();
}

void loop()
{
  int dist1 = 0;
  int dist2 = 0;
  int dist3 = 0;
  dist1 = getDistance(triggerPin, echoPin);
  dist2 = getDistance(triggerPin2, echoPin2);
  dist3 = getDistance(triggerPin3, echoPin3);
  printToSerial(1, dist1);
  printToSerial(2, dist2);
  printToSerial(3, dist3);
  delay(200);
  printToDisplay(1, dist1);
  printToDisplay(2, dist2);
  printToDisplay(3, dist3);
  display.display();
  display.clearDisplay();
  display.setCursor(0,0);
  makeLEDs();
}

void printToSerial(int index, int distance)
{
  char format[16];
  sprintf(format, "Distance %i: %i \n", index, distance);
  Serial.print(format);
}

void printToDisplay(int index, int distance)
{
  char format[16];
  sprintf(format, "Distance %i: %i cm", index, distance);
  display.println(format);
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

void makeLEDs()
{
  for (int i = 0; i <= 20; i++) {
    leds[i] = CRGB ( 255, 255, 255);
    FastLED.show();
    delay(5);
  }
  for (int i = 20; i >= 0; i--) {
    leds[i] = CRGB ( 255, 0, 0);
    FastLED.show();
    delay(5);
  }
}
