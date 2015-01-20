// IMPORTANT: Don't forget to add the NeoPixel Library in the Spark IDE
#include "neopixel/neopixel.h"

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define GREEN 0
#define ORANGE 64
#define RED 85
#define START_COLOR 200

uint32_t lastPixelUpdate = 0;
uint32_t nextColor = START_COLOR;
uint32_t currentColor = START_COLOR;
bool fading = false;
uint8_t fadePos = 254;
int fadeDir = 1;
char lastCommand[64];

int setColor(String command);
int setBrightness(String command);
int rainbow(String waitStr);
int parseCi(String arg);

void setup() {
  Spark.function("ci", parseCi);
  
  // debugging:
  Spark.function("color", setColor);
  Spark.function("brightness", setBrightness);
  Spark.variable("lastCommand", lastCommand, STRING);
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
    if (millis() - lastPixelUpdate > 20UL) {
        lastPixelUpdate = millis();
        if (fading) {
            if (fadePos >= 255) {
                fadePos = 255;
                fadeDir = -3;
            }
            if (fadePos <= 84) fadeDir = 3;
            fadePos += fadeDir;
            strip.setBrightness(fadePos);
        }
        if (nextColor != currentColor) {
            if (currentColor < nextColor) currentColor++;
            if (currentColor > nextColor) currentColor--;
        }
        for (uint16_t i=0; i<strip.numPixels(); i++) {
            strip.setPixelColor(i, Wheel(currentColor));
        }
        strip.show();
    }
}

int parseCi(String arg) {
    int somethingInteresting = 0;
    int lastPos = 0;
    int commaPos = 0;

    arg.toCharArray(lastCommand, 64);

    do {
        commaPos = arg.indexOf(",", lastPos);
        if (commaPos == -1) {
            commaPos = arg.length();
        }
        String state = arg.substring(lastPos, commaPos);
        lastPos = commaPos + 1;

        // colors
        if (state.equals("SUCCESS")) {
            nextColor = GREEN;
            somethingInteresting++;
        } else if (state.equals("UNSTABLE")) {
            nextColor = ORANGE;
            somethingInteresting++;
        } else if (state.equals("FAILURE")) {
            nextColor = RED;
            somethingInteresting++;
        } 
        
        // fading
        if (state.equals("STARTED")) {
            fadePos = 255;
            fading = true;
            somethingInteresting++;
        } else if (state.equals("FINISHED")) {
            fading = false;
            strip.setBrightness(255);
            somethingInteresting++;
        }
    } while (lastPos < arg.length());
    return somethingInteresting;
}

int setColor(String color) {
  int pos = color.toInt();
  if (pos < 0 || pos > 255) {
      return -1;
  }
  nextColor = pos;
  return 0;
}

int setBrightness(String brightness) {
  int b = brightness.toInt();
  if (b < 0 || b > 255) {
      return -1;
  }
  strip.setBrightness(b);
  strip.show();
  return b;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
