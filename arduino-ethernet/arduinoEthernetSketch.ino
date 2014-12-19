#include <mthread.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

//
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


// lightning effect animation on your pixels
class Lightning : public Thread {
  public:
    Lightning();
  protected:
    bool loop();
};
Lightning::Lightning() {}
bool Lightning::loop() {
  lightning();
  long time = random(70, 360);
  Serial.print("sleep long: ");
  Serial.println(time);
  this->sleep(time);
  return true; 
}

// TODO DHCP
// Ethernet Webserver
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,178,177);
EthernetServer server(80);

class WebServer : public Thread {
  public:
    WebServer();
  protected:
    bool loop();
};
WebServer::WebServer() {}
bool WebServer::loop() {
  colorServer();
  return true; 
}

// crackle and fade to black animation
class Blackout : public Thread {
  public:
    Blackout();
  protected:
    bool loop();
};
Blackout::Blackout() {}
bool Blackout::loop() {
  blackout();
  return false; 
}
 
void setup() {
  
  // for serial monitor
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  main_thread_list->add_thread(new WebServer());
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  //setup the pixels
  strip.begin();
  strip.show();  

  // lighning indicates the WebServer is running
  main_thread_list->add_thread(new Lightning()); 
}

void colorServer() {
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String req = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        req+=c;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          int r,g,b = 0;
          // req should contain "GET /color/"
          if (req.indexOf("color/") > -1) {
          	// TODO /color/AABBCC/
            // RGB Values from 0 to 255 like "GET /color/rrrgggbbb"
            char s[4] = {req.charAt(11),req.charAt(12),req.charAt(13)};
            r= String(s).toInt();
            char t[4] = {req.charAt(14),req.charAt(15),req.charAt(16)};
            g= String(t).toInt();
            char u[4] = {req.charAt(17),req.charAt(18),req.charAt(19)};
            b= String(u).toInt();
            setColor(r, g, b);
          };
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println(r);
          client.println(g);
          client.println(b);
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

//NEOPIXEL ////
void setColor(int r, int g, int b) {
  for(int i = 0 ; i <= 8; i +=1) { 
        strip.setBrightness(255);
        strip.setPixelColor(i, strip.Color(r, g, b));
        strip.show();        
  }
}

uint32_t a = strip.Color(10, 30, 75);
uint32_t b = strip.Color(120, 180, 100);

// the magic of randomness
// r is is a variating global value, which should stay between 0 and 255
int r = 175;
void variate() {
  int varianz = 150;
  r = r - (varianz / 2);
  r = r + random(varianz);
  if (r>255) {
    r = 255;
  }
  if (r<0) {
    r=0;
  }
}

void lightning() {
  
  variate();
	int brightness = random(20, 40);
	int crackles = random(15, 35);
	int fade = (255 - brightness) / (crackles - 3);
  for(int i = 0 ; i <= crackles; i +=1) { 
    brightness += fade;
    strip.setBrightness(brightness);
    strip.setPixelColor(random(0,8), a - (b/20));
    strip.setPixelColor(random(0,8), strip.Color(10,30,10+r));
    strip.show();
    delay(random(30,100));                            
  } 
    variate();
    // fade in from min to max in increments of 5 points:
  for(int j = 0 ; j <= crackles; j +=1) { 
    brightness -= fade;
    strip.setBrightness(brightness);
    strip.setPixelColor(random(0,8), a + (b/20));
    strip.setPixelColor(random(0,8), strip.Color(10,255-r,10+r));
    strip.show();
    delay(random(10,50));                            
  } 
  main_thread_list->add_thread(new Blackout());    
  
}
void blackout() {
   
  variate();
   
  for(int brightness = 120 ; brightness > 0; brightness -= 4) { 
    strip.setBrightness(brightness);
    strip.setPixelColor(random(0,8), a + (b/20));
    strip.setPixelColor(random(0,8), strip.Color(10,255-r,10+r));
    strip.show();
    delay(20);
  } 
  for (int i; i <8 ; i+=1) {
     strip.setBrightness(0);
     strip.setPixelColor(i, strip.Color(0,0,0));
  }  
  strip.show();                           
}
