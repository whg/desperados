
#define NOTE_ON 0b10010000
#define NOTE_OFF 0b10000000

//#define DEBUG


const int hitPins[7] = { 2, 3, 4, 5, 6, 7, 8 };
const int numPins = sizeof(hitPins) / sizeof(int);
const int noteNumbers[] = { 37, 38, 39, 40, 41, 42, 59 };

enum state_t { ON, OFF };
state_t states[numPins];

unsigned long lastHits[numPins];
const unsigned long HIT_THRESHOLD = 20; // millis

const unsigned long NOTE_DURATION = 50;

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

    if (states[i] == OFF && digitalRead(hitPins[i]) == 0) {
      if ((timeNow - lastHits[i]) > NOTE_DURATION && states[i] == OFF) {
        sendMidi(NOTE_ON, 1, noteNumbers[i], 80);
        lastHits[i] = timeNow;
        states[i] = ON;
        digitalWrite(LED_PIN, HIGH);
      }
    }
  }
  
  for (uint8_t i = 0; i < numPins; i++) {
    if (states[i] == ON && (timeNow - lastHits[i]) > NOTE_DURATION) {
      sendMidi(NOTE_OFF, 1, noteNumbers[i], 0);
      states[i] = OFF;

      digitalWrite(LED_PIN, LOW);
    }
  }
  
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

