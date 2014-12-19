#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

// from pin 6 to your pixels Data Input
#define PIN 6
// how many pixels should animate, can be lower than the number of attached pixels
int maxPixels = 8;
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(maxPixels, PIN, NEO_GRB + NEO_KHZ800);


// Ethernet Webserver
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,178,177);
EthernetServer server(80);

 
void setup() {
  
  // for serial monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  //setup the pixels
  strip.begin();
  strip.show();  
  setColor(255, 0, 0);
}

void setColor(int r, int g, int b) {
    strip.setBrightness(255); 
  for(int i = 0 ; i <= maxPixels; i +=1) { 
    strip.setPixelColor(i, strip.Color(r, g, b));
    strip.show();
    delay(250);                            
  } 
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      setColor(0, 255, 0);
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // req should contain "GET /color/"
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
