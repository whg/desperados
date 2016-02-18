#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 3

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(48, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

int LHTapValue = 0;
int RHTapValue = 0;
int previousLHTapValue = 0;
int previousRHTapValue = 0;
int LHled = 0;
int RHled = 0;
int LHledBrightness = 255;
int RHledBrightness = 255;

int LHnote;
int LHnoteLength = 50;
int LHnoteTurnedOn = 0;
int RHnote;
int RHnoteLength = 50;
int RHnoteTurnedOn = 0;
int analogueInJitterSmoothing = 5;

void setup() {


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Serial.begin(9600);
  Serial.begin(31250);
}

void loop() {

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - LEDs & MIDI for LH tap

  LHTapValue = analogRead(A0);

  if (LHTapValue > (previousLHTapValue + analogueInJitterSmoothing))
  {
    LHledBrightness = 255;
    LHled++;
    strip.setPixelColor((LHled - 1), 0, 0, 0);
    if (LHled > 23) {
      LHled = 0;
    }
    strip.setPixelColor(LHled, 255, 255, 255);
    strip.show();
    previousLHTapValue = LHTapValue;
    LHnoteTurnedOn = 1;
  }

  if (LHTapValue < (previousLHTapValue - analogueInJitterSmoothing))
  {
    LHledBrightness = 255;
    LHled--;
    strip.setPixelColor((LHled + 1), 0, 0, 0);
    if (LHled < 0) {
      LHled = 23;
    }
    strip.setPixelColor(LHled, 255, 255, 255);
    strip.show();
    previousLHTapValue = LHTapValue;
    LHnoteTurnedOn = 1;
  }

  if ((LHTapValue < (previousLHTapValue + analogueInJitterSmoothing)) && (LHTapValue > (previousLHTapValue - analogueInJitterSmoothing)))
  {
    LHledBrightness--;
    if (LHledBrightness < 1) {
      LHledBrightness = 0;
    }
    //delayMicroseconds(250);
    strip.setPixelColor(LHled, LHledBrightness, LHledBrightness, LHledBrightness);
    strip.show();

  }

  LHnote = (LHTapValue / 8);
  
  if (LHnoteTurnedOn == 1)
  {
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(0x90, LHnote, 0x45);
    LHnoteTurnedOn = 0;
  }






  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - LEDs & MIDI note length for RH tap

  RHTapValue = analogRead(A1);
  


  if (RHTapValue > (previousRHTapValue + analogueInJitterSmoothing))
  {
    RHledBrightness = 255;
    RHled++;
    strip.setPixelColor((47 - (RHled - 1)), 0, 0, 0);
    if (RHled > 23) {
      RHled = 0;
    }
    strip.setPixelColor((47 - RHled), 255, 255, 255);
    strip.show();
    previousRHTapValue = RHTapValue;
    RHnoteTurnedOn = 1;

  }

  if (RHTapValue < (previousRHTapValue - analogueInJitterSmoothing))
  {
    RHledBrightness = 255;
    RHled--;
    strip.setPixelColor((47 - (RHled + 1)), 0, 0, 0);
    if (RHled < 0) {
      RHled = 23;
    }
    strip.setPixelColor((47 - RHled), 255, 255, 255);
    strip.show();
    previousRHTapValue = RHTapValue;
    RHnoteTurnedOn = 1;

  }

  if ((RHTapValue < (previousRHTapValue + analogueInJitterSmoothing)) && (RHTapValue > (previousRHTapValue - analogueInJitterSmoothing)))
  {
    RHledBrightness--;
    if (RHledBrightness < 1) {
      RHledBrightness = 0;
    }
    //delayMicroseconds(250);
    strip.setPixelColor((47 - RHled), RHledBrightness, RHledBrightness, RHledBrightness);
    strip.show();
  }

  RHnote = (RHTapValue / 8);
  
  if (RHnoteTurnedOn == 1)
  {
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(0x91, RHnote, 0x45);
    RHnoteTurnedOn = 0;
  }




}




void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}


