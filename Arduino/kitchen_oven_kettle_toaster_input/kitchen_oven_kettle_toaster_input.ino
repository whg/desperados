#include <Encoder.h>

#define NOTE_ON 0b10010000
#define NOTE_OFF 0b10000000
#define CONTROL_CHANGE 0b10110000
#define PITCH_BEND 0b11100000

// 1024 / 127 = 8
#define KNOB_THRESHOLD 8

const int setStartButton = 2;
const int setEndButton = 3;

const int ovenEncoder2CCNumber = 22; // encoder 1 is pitch bend
const int ovenPot1CCNumber = 20;
const int ovenPot2CCNumber = 21;
const int toasterCCNumber = 70;
const int kettleCCNumber = 23;

Encoder ovenEncoder1(4, 5);
Encoder ovenEncoder2(7, 8);

const int ovenPot1Pin = A0;
const int ovenPot2Pin = A1;
int ovenPot1Val, ovenPot2Val;

const int toasterPin = A3;
int toasterVal;

const int kettlePin = 10;
int kettleVal;

long ovenEncoder1Pos, ovenEncoder1Start, ovenEncoder1End;
long ovenEncoder2Pos, ovenEncoder2Start, ovenEncoder2End;

bool readOvenEncoder1() {
  static long oldPos1 = -999;
  ovenEncoder1Pos = ovenEncoder1.read();
  if (ovenEncoder1Pos != oldPos1) {
    oldPos1 = ovenEncoder1Pos;
    return true;
  }
  return false;
}


bool readOvenEncoder2() {
  static long oldPos2 = -999;
  ovenEncoder2Pos = ovenEncoder2.read();
  if (ovenEncoder2Pos != oldPos2) {
    oldPos2 = ovenEncoder2Pos;
    return true;
  }
  return false;
}

#define ENCODER_NOT_SET -999
#define ENCODER_VAL_SET(encoderVal) ((encoderVal != ENCODER_NOT_SET))

void setup() {

  Serial.begin(31250);

  pinMode(setStartButton, INPUT);
  pinMode(setEndButton, INPUT);

  pinMode(toasterPin, INPUT);
  pinMode(kettlePin, INPUT_PULLUP);

  pinMode(ovenPot1Pin, INPUT);
  pinMode(ovenPot2Pin, INPUT);

  ovenEncoder1Start = ovenEncoder1End = ENCODER_NOT_SET;
  ovenEncoder2Start = ovenEncoder2End = ENCODER_NOT_SET;

  ovenPot1Val = ovenPot2Val = 0;
  toasterVal = kettleVal = 0;
}


void loop() {

  bool startButtonPressed= digitalRead(setStartButton) == HIGH;
  bool endButtonPressed = digitalRead(setEndButton) == HIGH;

  bool encoder1Changed = readOvenEncoder1();
  bool encoder2Changed = readOvenEncoder2();
  
  
  if (startButtonPressed) {
    ovenEncoder1Start = ovenEncoder1Pos;
    ovenEncoder2Start = ovenEncoder2Pos;
  }
  else if (endButtonPressed) {
    ovenEncoder1End = ovenEncoder1Pos;
    ovenEncoder2End = ovenEncoder2Pos;
  }
  
  if (encoder1Changed && ENCODER_VAL_SET(ovenEncoder1Start) && ENCODER_VAL_SET(ovenEncoder1End)) {
    uint16_t value = constrain(map(ovenEncoder1Pos, ovenEncoder1Start, ovenEncoder1End, 0, 0x2000), 0, 0x2000);
    sendPitchBend(1, value);
  }

  if (encoder2Changed && ENCODER_VAL_SET(ovenEncoder2Start) && ENCODER_VAL_SET(ovenEncoder2End)) {
    uint8_t value = constrain(map(ovenEncoder2Pos, ovenEncoder2Start, ovenEncoder2End, 0, 127), 0, 127);
    sendMidi(CONTROL_CHANGE, 1, ovenEncoder2CCNumber, value);
  }


  int p1v = analogRead(ovenPot1Pin);
  if (abs(p1v- ovenPot1Val) > KNOB_THRESHOLD) {
    ovenPot1Val = p1v;
    sendMidi(CONTROL_CHANGE, 1, ovenPot1CCNumber, map(ovenPot1Val, 0, 1024, 0, 127));
  }

  int p2v = analogRead(ovenPot2Pin);
  if (abs(p2v- ovenPot2Val) > KNOB_THRESHOLD) {
    ovenPot2Val = p2v;
    sendMidi(CONTROL_CHANGE, 1, ovenPot2CCNumber, map(ovenPot2Val, 0, 1024, 0, 127));
  }

  int tv = analogRead(toasterPin);
  if (abs(tv - toasterVal) > KNOB_THRESHOLD) {
    toasterVal = tv;
    sendMidi(CONTROL_CHANGE, 1, toasterCCNumber, map(toasterVal, 0, 1024, 0, 127));
  }

  int kv = digitalRead(kettlePin);
  
  if (kv != kettleVal) {
    kettleVal = kv;
    sendMidi(CONTROL_CHANGE, 1, kettleCCNumber, kv == LOW ? 127 : 0);
  }

  delay(30);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
}

void sendPitchBend(uint8_t channel, uint16_t value) {
  byte cmd = PITCH_BEND | ((channel-1) & 15);
  Serial.write(cmd);

  value+= 0x2000;
//  value = 255;
  
  Serial.write(value & 127);
  Serial.write((value >> 7) & 127);
}


