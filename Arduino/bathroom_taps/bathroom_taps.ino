
#define CONTROL_CHANGE 0b10110000
// 1024 / 127 = 8
#define KNOB_THRESHOLD 8

#define DEBUG

const int bathSink1CCNumber = 16;
const int bathSink2CCNumber = 17;

const int bathSink1Pin = A0;
const int bathSink2Pin = A1;
int bathSink1Val, bathSink2Val;

const int tap1LightPins[] = { 2, 3, 4 };
const int tap2LightPins[] = { 5, 6, 7 };
int tap1MidiVal, tap2MidiVal;

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#else
  Serial.begin(31250);
#endif

  pinMode(bathSink1Pin, INPUT);
  pinMode(bathSink2Pin, INPUT);

  for (int i = 0; i < 3; i++) {
    pinMode(tap1LightPins[i], OUTPUT);
    pinMode(tap2LightPins[i], OUTPUT);
  }

  bathSink1Val = bathSink2Val = 0;
  tap1MidiVal = tap2MidiVal = 0;
}


void loop() {

  for (int i = 0; i < 3; i++) {
    digitalWrite(tap1LightPins[i], i == ((tap1MidiVal/3) % 3) ? HIGH : LOW);
    digitalWrite(tap2LightPins[i], i == ((tap2MidiVal/3) % 3) ? HIGH : LOW);
  }
  
  int p1v = analogRead(bathSink1Pin);
  static int p1OverTimes = 0;
  if (abs(p1v- bathSink1Val) > KNOB_THRESHOLD) {
    p1OverTimes++;
  }
  else {
    p1OverTimes = 0;
  }

  if (p1OverTimes > 10) {
    bathSink1Val = p1v;
    tap1MidiVal = map(bathSink1Val, 0, 1024, 0, 127);
    sendMidi(CONTROL_CHANGE, 1, bathSink1CCNumber, tap1MidiVal);
  }

#ifdef DEBUG
  Serial.println(tap1MidiVal);
#endif

  int p2v = analogRead(bathSink2Pin);
  static int p2OverTimes = 0;
  if (abs(p2v- bathSink2Val) > KNOB_THRESHOLD) {
    p2OverTimes++;
  }
  else {
    p2OverTimes = 0;
  }

  if (p2OverTimes > 10) {
    bathSink2Val = p2v;
    tap2MidiVal = map(bathSink2Val, 0, 1024, 0, 127);
    sendMidi(CONTROL_CHANGE, 1, bathSink2CCNumber, tap2MidiVal);
  }

  delay(1);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
#ifndef DEBUG
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
#endif
}



