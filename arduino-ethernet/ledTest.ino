#include <Adafruit_NeoPixel.h>

#define PIN 6
// how many pixels should animate, can be lower than the number of attached pixels
int maxPixels = 8;
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(maxPixels, PIN, NEO_GRB + NEO_KHZ800);
 
void setup() {  
  strip.begin();
  strip.show();  
}

void loop() {
  strip.setBrightness(255); 
  for (int i = 0 ; i <= maxPixels; i +=1) { 
    strip.setPixelColor(i, strip.Color(0,0,255));
    strip.show();
    delay(250);                            
  } 
}