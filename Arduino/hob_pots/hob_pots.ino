#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 2
#define NUMPIXELS      16
#define CONTROL_CHANGE 0b10110000
// 1024 / 127 = 8
#define KNOB_THRESHOLD 8

//#define DEBUG

const int hobPot1CCNumber = 18;
const int hobPot2CCNumber = 19;

const int hobPot1Pin = A0;
const int hobPot2Pin = A3;
int hobPot1Val, hobPot2Val;

const int vuMeterPin = A1;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

static uint32_t vuColours[8] {
    pixels.Color(0, 255, 0),
    pixels.Color(0, 255, 0),
    pixels.Color(255, 255, 0),
    pixels.Color(255, 255, 0),
    pixels.Color(255, 255, 0),
    pixels.Color(255, 255, 0),
    pixels.Color(255, 0, 0),
    pixels.Color(255, 0, 0)
  };

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#else
  Serial.begin(31250);
#endif

  pinMode(hobPot1Pin, INPUT);
  pinMode(hobPot2Pin, INPUT);
  pinMode(vuMeterPin, INPUT);
  
  hobPot1Val = hobPot2Val = 0;
  
  pixels.begin();
}


void loop() {
  
  int p1v = analogRead(hobPot1Pin);
  static int p1OverTimes = 0;
  if (abs(p1v- hobPot1Val) > KNOB_THRESHOLD) {
    p1OverTimes++;
  }
  else {
    p1OverTimes = 0;
  }

  if (p1OverTimes > 10) {
    hobPot1Val = p1v;
    sendMidi(CONTROL_CHANGE, 1, hobPot1CCNumber, map(hobPot1Val, 0, 1024, 0, 127));
  }

#ifdef DEBUG
  Serial.println(p1v);
#endif

  int p2v = analogRead(hobPot2Pin);
  static int p2OverTimes = 0;
  if (abs(p2v- hobPot2Val) > KNOB_THRESHOLD) {
    p2OverTimes++;
  }
  else {
    p2OverTimes = 0;
  }

  if (p2OverTimes > 10) {
    hobPot2Val = p2v;
    sendMidi(CONTROL_CHANGE, 1, hobPot2CCNumber, map(hobPot2Val, 0, 1024, 0, 127));
  }

  int vuMeterReading = map(analogRead(vuMeterPin), 0, 800, 0, 8);
  uint32_t col;
  for (int i = 0; i < 8; i++) {
    if (i < vuMeterReading) col = vuColours[i];
    else col = 0;
    pixels.setPixelColor(i, col);
    pixels.setPixelColor(8 + i, col);
  }

  delay(1);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
}



