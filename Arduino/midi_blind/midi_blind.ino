#include <Encoder.h>

const int blindCCNumber = 2;

const int setStartButton = 2;
const int setEndButton = 3;

#define NOTE_ON 0b10010000
#define NOTE_OFF 0b10000000
#define CONTROL_CHANGE 0b10110000

Encoder encoder(A0, A1);
long encoderPos, encoderStart, encoderEnd; 

bool readEncoder() {
  static long oldPos = -999;
  encoderPos = encoder.read();
  if (encoderPos != oldPos) {
    oldPos = encoderPos;
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

  encoderStart = encoderEnd = ENCODER_NOT_SET;
}


void loop() {

  bool startButtonPressed= digitalRead(setStartButton) == HIGH;
  bool endButtonPressed = digitalRead(setEndButton) == HIGH;

  bool encoderChanged = readEncoder();
  
  
  if (startButtonPressed) {
    encoderStart = encoderPos;
  }
  else if (endButtonPressed) {
    encoderEnd = encoderPos;
  }
  else if (encoderChanged && ENCODER_VAL_SET(encoderStart) && ENCODER_VAL_SET(encoderEnd)) {
    uint8_t value = constrain(map(encoderPos, encoderStart, encoderEnd, 0, 127), 0, 127);
    sendMidi(CONTROL_CHANGE, 1, blindCCNumber, value);
  }


}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1);
  Serial.write(byte2);
}

void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  sendMidi(NOTE_ON, channel, pitch, velocity);
}

//void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
//  byte cmd = NOTE_ON | ((channel-1) & 15);
//  Serial.write(cmd);
//  Serial.write(pitch);
//  Serial.write(velocity);
//}

void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  byte cmd = CONTROL_CHANGE | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(control);
  Serial.write(value);
}

