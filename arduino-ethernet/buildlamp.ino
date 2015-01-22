#include <mthread.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

#define PIXEL_PIN 6 // FIXME: number of pin you connected the neopixels to
#define PIXEL_COUNT 8 // FIXME: number of neopixels connected
// Your Arduino should have a MAC address printed on a sticker
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x00, 0x00 }; // FIXME: mac address

#define WEBDUINO_FAIL_MESSAGE "<h1>Whoops :(</h1>"
#define WEBDUINO_FAVICON_DATA ""
#define WEBDUINO_SERIAL_DEBUGGING 1
#include <WebServer.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
WebServer webserver;

#define GREEN 0
#define ORANGE 64
#define RED 85
#define START_COLOR 200

uint32_t lastPixelUpdate = 0;
uint32_t currentColor = START_COLOR;
uint32_t nextColor = START_COLOR;
boolean fading = false;
int fadeDir = 1;
int fadePos = 254;

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


class LightThread : public Thread {
public:
  LightThread();
protected:
  bool loop();
};

LightThread::LightThread() {}
bool LightThread::loop() {
  if (kill_flag) return false;
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
  return true; 
}

class ServerThread : public Thread {
public:
  ServerThread();
protected:
  bool loop();
};

ServerThread::ServerThread() {}
bool ServerThread::loop() {
  if (kill_flag) return false;
  webserver.processConnection();
  return true; 
}

void ci(WebServer &server, WebServer::ConnectionType type, char *, bool) {
  if (type != WebServer::POST) {
    P(httpStatus) = "HTTP/1.0 405 Method Not Allowed" CRLF;
    P(otherHeaders) = "Accept: POST" CRLF "Content-Type: text/html" CRLF CRLF;
    server.printP(httpStatus);
    server.printP(otherHeaders);
    P(humanStatus) = "<h1>Method Not Allowed</h1><p>Use POST.</p>" CRLF;
    server.printP(humanStatus);
    return;
  }
  
  String args;
  bool repeat;
  char name[16], value[64];
  do {
    repeat = server.readPOSTparam(name, 16, value, 64);
    if (strcmp(name, "args") == 0) {
      args = value;
    }
  } while (repeat);

  if (args.length() == 0) {
    server.httpFail();
    P(argsError) = "<p>No argument found. Use <code>&args=...</code></p>" CRLF;
    server.printP(argsError);
    return; 
  }

  int somethingInteresting = 0;
  int lastPos = 0;
  int commaPos = 0;

  do {
    commaPos = args.indexOf(",", lastPos);
    if (commaPos == -1) {
      commaPos = args.length();
    }
    String state = args.substring(lastPos, commaPos);
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
  } while (lastPos < args.length());
  
  server.httpSuccess("application/json");
  server.printf("{\"returnValue\": %s}", somethingInteresting);
  return;
}

void hello(WebServer &server, WebServer::ConnectionType type, char *, bool) {
  server.httpSuccess("text/html");
  P(ohai) = "<h1>buildlamp</h1>" CRLF;
  server.printP(ohai);
  return;
}

void setup() {
  Serial.begin(9600);
  strip.begin();
  
  // FIXME: color for connecting.
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac);

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // FIXME: color for failure.
    for(;;)
      ;
  }

  Serial.print("[Buildlamp] HTTP API is at ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();

  webserver.setDefaultCommand(&hello);
  webserver.addCommand("ci", &ci);
  webserver.begin();
  
  main_thread_list->add_thread(new ServerThread());
  main_thread_list->add_thread(new LightThread()); 
}
