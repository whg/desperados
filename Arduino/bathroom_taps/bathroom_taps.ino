
#define CONTROL_CHANGE 0b10110000
// 1024 / 127 = 8
#define KNOB_THRESHOLD 8

//#define DEBUG

const int bathSink1CCNumber = 16;
const int bathSink2CCNumber = 17;

const int bathSink1Pin = A0;
const int bathSink2Pin = A1;
int bathSink1Val, bathSink2Val;

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#else
  Serial.begin(31250);
#endif

  pinMode(bathSink1Pin, INPUT);
  pinMode(bathSink2Pin, INPUT);

  bathSink1Val = bathSink2Val = 0;
}


void loop() {

  
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
    sendMidi(CONTROL_CHANGE, 1, bathSink1CCNumber, map(bathSink1Val, 0, 1024, 0, 127));
  }

#ifdef DEBUG
  Serial.println(p1v);
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
    sendMidi(CONTROL_CHANGE, 1, bathSink2CCNumber, map(bathSink2Val, 0, 1024, 0, 127));
  }

  delay(1);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
}



