#include <Encoder.h>

#define NOTE_ON 0b10010000
#define NOTE_OFF 0b10000000

// in millis
#define HOLD_TIME 20

const int notes[2] = { 36, 37 };
const int pins[2] = { 2, 3 };
const int channels[2] = { 2, 3 };
const int NPINS = sizeof(notes) / sizeof(int);

byte states[NPINS];

void setup() {

  Serial.begin(31250);

  for (int i = 0; i < NPINS; i++) {
    pinMode(pins[i], INPUT);
    states[i] = HIGH;
  }
}

void loop() {
  
  for (int i = 0; i < NPINS; i++) {
    int v = digitalRead(pins[i]);
    if (v == HIGH && states[i] != HIGH) {
      sendMidi(NOTE_ON, channels[i], notes[i], 127);
      states[i] = HIGH;
      delay(HOLD_TIME);  
    }
    else if (v == LOW && states[i] == HIGH) {
      sendMidi(NOTE_OFF, channels[i], notes[i], 0);
      states[i] = LOW;
      delay(HOLD_TIME);
      
    }
    
  }

}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel - 1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
}

