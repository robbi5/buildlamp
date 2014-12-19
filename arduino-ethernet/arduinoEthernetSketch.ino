//Threads
//
//
#include <mthread.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetServer server(80);

class MainThread : public Thread {
public:
  MainThread();
protected:
  bool loop();
};

//Definition des Konstruktors
MainThread::MainThread()
{
}

//Definition der Threadschleife
bool MainThread::loop()
{
  // Die if requested:
  if(kill_flag)
    return false;
  
  sparkle();
  Serial.print("sleep long");
  this->sleep_micro(random(70000,200000));
  delay(5000);
  return true; 
}

class ColorListener : public Thread {
public:
  ColorListener();
protected:
  bool loop();
};

//Definition des Konstruktors
ColorListener::ColorListener()
{
}

//Definition der Threadschleife
bool ColorListener::loop()
{
  // Die if requested:
  if(kill_flag)
    return false;
  
  colorServer();

  return true; 
}

class Blackout : public Thread {
public:
  Blackout();
protected:
  bool loop();
};

//Definition des Konstruktors
Blackout::Blackout()
{
}

//Definition der Threadschleife
bool Blackout::loop()
{
  // Die if requested:
  if(kill_flag)
    return false;
  
  blackout();

  return false; 
}

void setup() {
  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  Serial.begin(9600);
  main_thread_list->add_thread(new ColorListener());
  main_thread_list->add_thread(new MainThread()); 
  
  
  strip.begin();
  uint32_t a = strip.Color(random(10,200),random(10,200),random(10,200));
  strip.setPixelColor(random(0,6), a);
  strip.show();  
}



// SERVER ////

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
          Serial.println(req);
          // req should contein "GET /?color="
          if (req.indexOf("color=") > -1) {
            // RGB Values from 0 to 255 like "GET /?color=rrrgggbbb"
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
    delay(100);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

int getValue(String s) {
  return s.toInt();
}

//NEOPIXEL ////

void setColor(int r, int g, int b) {
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);
  for(int i = 0 ; i <= 8; i +=1) { 
    strip.setBrightness(255);
    strip.setPixelColor(i, strip.Color(r, g, b));
    strip.show();        
  }
}


void thriller(){
  
  for (int j = 0 ; j<=20; j+=3) {
    int i = random(j, 20);  
    for(int fadeValue = 0 ; fadeValue <= 5; fadeValue +=1) { 
      strip.setPixelColor(fadeValue, strip.Color(i*3,100 - 20*i,2*i +50));
      strip.setPixelColor(5-fadeValue, strip.Color(i*10, 200 - 20*i,2*i +10));
      strip.show();        
      delay(30);                            
    }    
    
  }
}

int r = 175;
uint32_t a = strip.Color(random(10,200)+40,random(10,200),random(10,200));
uint32_t b = strip.Color(220, 220, 220);


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

void sparkle() {
  
  variate();
  int br = random(20, 40);
  int crackles = random(15, 35);
  int fade = (255 - br) / (crackles - 3);
  
  
  for(int i = 0 ; i <= crackles; i +=1) { 
    br += fade;
    strip.setBrightness(br);
    strip.setPixelColor(random(0,8), a - (b/20));
    strip.setPixelColor(random(0,8), strip.Color(10,30,10+r));
    strip.show();
    delay(random(30,100));                            
  } 
  variate();
    // fade in from min to max in increments of 5 points:
  for(int j = 0 ; j <= crackles; j +=1) { 
    br -= fade;
    strip.setBrightness(br);
    strip.setPixelColor(random(0,8), a + (b/20));
    strip.setPixelColor(random(0,8), strip.Color(10,255-r,10+r));
    strip.show();
    delay(random(10,50));                            
  } 
  main_thread_list->add_thread(new Blackout());    
  
}
void blackout() {
 
  variate();
  
  for(int br = 120 ; br > 0; br -= 4) { 
    strip.setBrightness(br);
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