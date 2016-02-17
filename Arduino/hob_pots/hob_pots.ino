
#define CONTROL_CHANGE 0b10110000
// 1024 / 127 = 8
#define KNOB_THRESHOLD 8

const int hobPot1CCNumber = 18;
const int hobPot2CCNumber = 19;

const int hobPot1Pin = A0;
const int hobPot2Pin = A3;
int hobPot1Val, hobPot2Val;

void setup() {

  Serial.begin(31250);

  pinMode(hobPot1Pin, INPUT);
  pinMode(hobPot2Pin, INPUT);

  hobPot1Val = hobPot2Val = 0;
}


void loop() {

  
  int p1v = analogRead(hobPot1Pin);
  if (abs(p1v- hobPot1Val) > KNOB_THRESHOLD) {
    hobPot1Val = p1v;
    sendMidi(CONTROL_CHANGE, 1, hobPot1CCNumber, map(hobPot1Val, 0, 1024, 0, 127));
  }

  int p2v = analogRead(hobPot2Pin);
  if (abs(p2v- hobPot2Val) > KNOB_THRESHOLD) {
    hobPot2Val = p2v;
    sendMidi(CONTROL_CHANGE, 1, hobPot2CCNumber, map(hobPot2Val, 0, 1024, 0, 127));
  }
sendMidi(CONTROL_CHANGE, 1, hobPot1CCNumber, map(p1v, 0, 1024, 0, 127));
  delay(10);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
}



