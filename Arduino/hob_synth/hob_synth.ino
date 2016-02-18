
#define NOTE_ON 0b10010000
#define NOTE_OFF 0b10000000

//#define DEBUG


const int hitPins[4] = { A0, A1, A2, A3 };
const int numPins = sizeof(hitPins) / sizeof(int);
const int noteNumbers[] = { 29, 31, 32, 34 };
const int midiChannel = 5;

enum state_t { ON, OFF };
state_t states[numPins];

unsigned long lastHits[numPins];
const unsigned long HIT_THRESHOLD = 20; // millis 
const int NOTE = 24; // C0
const unsigned long NOTE_DURATION = 30;

const int LED_PIN = 13;

void setup() {

#ifdef DEBUG
  Serial.begin(9600);
#else
  Serial.begin(31250);
#endif
  for (uint8_t i = 0; i < numPins; i++) {
    pinMode(hitPins[i], INPUT);
    lastHits[i] = 0;
    states[i] = OFF;
  }

  pinMode(LED_PIN, OUTPUT);
}


void loop() {

  unsigned long timeNow = millis();
 
  for (uint8_t i = 0; i < numPins; i++) {
    if (states[i] == OFF && analogRead(hitPins[i]) > 500) {
      if ((timeNow - lastHits[i]) > NOTE_DURATION) {
        sendMidi(NOTE_ON, midiChannel, noteNumbers[i], 120);
        lastHits[i] = timeNow;
        states[i] = ON;
       // Serial.println(analogRead(hitPins[i]));
        digitalWrite(LED_PIN, HIGH);
      }
    }
  }
  
  for (uint8_t i = 0; i < numPins; i++) {
    if (states[i] == ON && (timeNow - lastHits[i]) > NOTE_DURATION && analogRead(hitPins[i]) < 500) {
      sendMidi(NOTE_OFF, midiChannel, noteNumbers[i], 0);
      states[i] = OFF;

      digitalWrite(LED_PIN, LOW);
    }
  }

  
  delay(1);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {

#ifdef DEBUG
  if (type == NOTE_ON) Serial.print("Note on: ");
  else if (type == NOTE_OFF) Serial.print("Note off: ");
  Serial.print(channel);
  Serial.print(", ");
  Serial.println(byte1);
#else
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1);
  Serial.write(byte2);
#endif
}


