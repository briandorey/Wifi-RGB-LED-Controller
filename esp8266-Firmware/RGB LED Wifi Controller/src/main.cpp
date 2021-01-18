#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FastLED.h>
#include <main.h>
#define NUM_LEDS 350

#ifndef STASSID
#define STASSID "your wifi network"
#define STAPSK  "your wifi password"
#endif


CRGBArray<NUM_LEDS> leds;


#define PIN D4

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

// Set your Static IP address
IPAddress local_IP(10, 0, 0, 175);
// Set your Gateway IP address
IPAddress gateway(10, 0, 0, 0);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

// Start Twinkle Mode
// Effect from https://github.com/atuline/FastLED-Demos
CRGBPalette16 currentPalette = PartyColors_p;
CRGBPalette16 targetPalette;
TBlendType    currentBlending = LINEARBLEND;
// End Twinkle Mode

// Start breath mode
// Effect From
// https://github.com/marmilicious/FastLED_examples/blob/master/breath_effect_v2.ino
static float pulseSpeed = 0.5;  // Larger value gives faster pulse.

uint8_t hueA = 15;  // Start hue at valueMin.
uint8_t satA = 230;  // Start saturation at valueMin.
float valueMin = 120.0;  // Pulse minimum value (Should be less then valueMax).

uint8_t hueB = 95;  // End hue at valueMax.
uint8_t satB = 255;  // End saturation at valueMax.
float valueMax = 255.0;  // Pulse maximum value (Should be larger then valueMin).

uint8_t hue = hueA;  // Do Not Edit
uint8_t sat = satA;  // Do Not Edit
float val = valueMin;  // Do Not Edit
uint8_t hueDelta = hueA - hueB;  // Do Not Edit
static float delta = (valueMax - valueMin) / 2.35040238;  // Do Not Edit

// End breath mode

// Start fade mode
int fadeAmount = 1;
bool fadeDirection = true; // true = up, false = down
int fadebrightness = 100;
// end fade mode


long color = 0;
const int led = 13;
bool BreakAction = false;
int DisplayMode = 0;
static uint8_t starthue = 0;
int BrightNess = 128;
void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {



  pinMode(led, OUTPUT);
  digitalWrite(led, 0);


  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( 0 );
  FastLED.clear(true);
  FastLED.show();
  //fadeToBlackBy(leds, NUM_LEDS, 0);


  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }


  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });



  server.on("/poweroff", []() {
    DisplayMode = 0;
    FastLED.setBrightness( 0 );
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.clear(true);
    FastLED.show();
    server.send(200, "text/plain", "poweroff");
  });

  server.on("/rainbow", []() {
    SetMode(1);
    server.send(200, "text/plain", "rainbow");
  });

  server.on("/twinkle", []() {
    SetMode(2);
    server.send(200, "text/plain", "twinkle");
  });

  server.on("/breath", []() {
    SetMode(3);
    server.send(200, "text/plain", "breath");
  });

  server.on("/glow", []() {
    DisplayMode = 4;
    server.send(200, "text/plain", "glow");
  });

  server.on("/chase", []() {
    DisplayMode = 5;
    server.send(200, "text/plain", "chase");
  });





  server.on("/setcolour", handleSetcolourArg);

  server.on("/setbrightness", handleSetBrightnessArg);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();

  // Rainbow mode
  if (DisplayMode == 1) {
    fill_rainbow( leds, NUM_LEDS, --starthue, 20);
      //FastLED.delay(30);
      FastLED.show();
      FastLED.delay(8);
  }

  if (DisplayMode == 2) {
      EVERY_N_MILLISECONDS(100) {                                                 // FastLED based non-blocking FIXED delay.
      uint8_t maxChanges = 24;
      nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);    // AWESOME palette blending capability.
    }

    EVERY_N_SECONDS(5) {                                                        // Change the target palette to a random one every 5 seconds.
      uint8_t baseC = random8(255);                                             // Use the built-in random number generator as we are re-initializing the FastLED one.
      targetPalette = CRGBPalette16(CHSV(baseC+random8(0,32), 255, random8(128, 255)), CHSV(baseC+random8(0,32), 255, random8(128, 255)), CHSV(baseC+random8(0,32), 192, random8(128, 255)), CHSV(baseC+random8(0,32), 255, random8(128, 255)));
    }

    fadein();
    FastLED.show();
  }

   if (DisplayMode == 3) {
    float dV = ((exp(sin(pulseSpeed * millis()/2000.0*PI)) -0.36787944) * delta);
    val = valueMin + dV;
    hue = map(val, valueMin, valueMax, hueA, hueB);  // Map hue based on current val
    sat = map(val, valueMin, valueMax, satA, satB);  // Map sat based on current val

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(hue, sat, val);

      // You can experiment with commenting out these dim8_video lines
      // to get a different sort of look.
      leds[i].r = dim8_video(leds[i].r);
      leds[i].g = dim8_video(leds[i].g);
      leds[i].b = dim8_video(leds[i].b);
    }

    FastLED.show();
   }

  // Glow
  if (DisplayMode == 4) {
    EVERY_N_MILLISECONDS(50) {
      if (fadeDirection) {
      fadebrightness++;
      if (fadebrightness >= 254) fadeDirection = false;
      }
      else {
        fadebrightness--;
        if (fadebrightness <= 1) fadeDirection = true;
      }
      FastLED.showColor(color, fadebrightness);
  }
}
}

void SetMode(int newmode) {
  FastLED.setBrightness( BrightNess );
  FastLED.clear();  // clear all pixel data
  FastLED.show();
  DisplayMode = newmode;
}

void fadein() {

  random16_set_seed(535);                                                           // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.

  for (int i = 0; i<NUM_LEDS; i++) {
    uint8_t fader = sin8(millis()/random8(10,20));                                  // The random number for each 'i' will be the same every time.
    leds[i] = ColorFromPalette(currentPalette, i*20, fader, currentBlending);       // Now, let's run it through the palette lookup.
  }

  random16_set_seed(millis());                                                      // Re-randomizing the random number seed for other routines.

}

void handleGenericArgs() { //Handler

  String message = "Number of args received:";
  message += server.args();            //Get number of parameters
  message += "\n";                            //Add a new line

  for (int i = 0; i < server.args(); i++) {

    message += "Arg nº" + (String)i + " –> ";   //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter

  }

  server.send(200, "text/plain", message);       //Response to the HTTP request

}

void handleSetBrightnessArg() {
  String message = "";
  if (server.arg("level") == "") {   //Parameter not found

    message = "level Argument not found";

  } else {    //Parameter found

    message = "Brightness Argument = ";
    message += server.arg("level");
    String colorString = "";  //Gets the value of the query parameter
    colorString = server.arg("level");
    BrightNess =  colorString.toInt();
    FastLED.setBrightness(colorString.toInt());
    //FastLED.show();

  }

  server.send(200, "text/plain", message);          //Returns the HTTP response


}

void handleSetcolourArg() {
  DisplayMode = 0;

  FastLED.clear();  // clear all pixel data
  FastLED.show();
  String message = "";
  if (server.arg("level") == "") {   //Parameter not found

    message = "level Argument not found";

  } else {    //Parameter found
    String levelString = "";  //Gets the value of the query parameter
    levelString = server.arg("level");
    BrightNess =  levelString.toInt();
    fadebrightness = BrightNess;
  }

  if (server.arg("colour") == "") {   //Parameter not found

    message = "colour Argument not found";

  } else {    //Parameter found

    message = "colour Argument = ";
    message += server.arg("colour");
    String colorString = "";  //Gets the value of the query parameter
    colorString = server.arg("colour");
    color = strtol(colorString.c_str(), NULL, 16);
    FastLED.showColor(color, BrightNess);
  }

  server.send(200, "text/plain", message);          //Returns the HTTP response

}
void setAllOnColour(char colour, int brightness) {
  memset(leds, 0, NUM_LEDS * 3);
  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    switch (colour) {
      case 'r': leds[i].r = brightness; break;
      case 'g': leds[i].g = brightness; break;
      case 'b': leds[i].b = brightness; break;
    }
  }
  FastLED.show();
}