#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

Adafruit_NeoPixel strip = Adafruit_NeoPixel(48, 3, NEO_GRB + NEO_KHZ800);

#define CONTROL_CHANGE 0b10110000
// 1024 / 127 = 8
#define KNOB_THRESHOLD 8

//#define DEBUG

const int kitchTap1CCNumber = 24;
const int kitchTap2CCNumber = 25;

const int kitchTap1Pin = A0;
const int kitchTap2Pin = A1;
int kitchTap1Val, kitchTap2Val;

int tap1MidiVal, tap1Brightness;
int tap2MidiVal, tap2Brightness;

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#else
  Serial.begin(31250);
#endif
  strip.begin();
  
  pinMode(kitchTap1Pin, INPUT);
  pinMode(kitchTap2Pin, INPUT);

  kitchTap1Val = kitchTap2Val = 0;
  tap1MidiVal = tap2MidiVal = 0;
  tap1Brightness = tap2Brightness = 0;
}


void loop() {

  for (int i = 0; i < 48; i++) {
    uint32_t c = strip.getPixelColor(i);
    uint8_t r = c & 255;
//    uint8_t g = (c>>8) & 255;
//    uint8_t b = (c>>16) & 255;
    strip.setPixelColor(i, 0,0,0);
  }
  
  int p1v = analogRead(kitchTap1Pin);
  static int p1OverTimes = 0;
  if (abs(p1v- kitchTap1Val) > KNOB_THRESHOLD) {
    p1OverTimes++;
  }
  else {
    p1OverTimes = 0;
    tap1Brightness = max(0, tap1Brightness-1);;
  }

  if (p1OverTimes > 10) {
    kitchTap1Val = p1v;
    tap1MidiVal = map(kitchTap1Val, 0, 1024, 0, 127);
    sendMidi(CONTROL_CHANGE, 1, kitchTap1CCNumber, tap1MidiVal);
    tap1Brightness = 255;
    strip.setPixelColor(tap1MidiVal % 24, 255, 255, 255);
  }

  strip.setPixelColor(tap1MidiVal % 24, tap1Brightness, tap1Brightness, tap1Brightness);

  int p2v = analogRead(kitchTap2Pin);
  static int p2OverTimes = 0;
  if (abs(p2v- kitchTap2Val) > KNOB_THRESHOLD) {
    p2OverTimes++;
  }
  else {
    p2OverTimes = 0;
    tap2Brightness = max(0, tap2Brightness-1);;
  }

  if (p2OverTimes > 10) {
    kitchTap2Val = p2v;
    tap2MidiVal = map(kitchTap2Val, 0, 1024, 0, 127);
    sendMidi(CONTROL_CHANGE, 1, kitchTap2CCNumber, tap2MidiVal);
    tap2Brightness = 255;
  }
  
  strip.setPixelColor(tap2MidiVal % 24 + 24, tap2Brightness, tap2Brightness, tap2Brightness);
  strip.show();
  delay(1);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
}



