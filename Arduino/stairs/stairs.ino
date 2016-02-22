
#define NOTE_ON 0b10010000
#define NOTE_OFF 0b10000000

#define C_2 0
#define Cs_2 1
#define D_2 2
#define Ds_2 3
#define E_2 4
#define F_2 5
#define Fs_2 6
#define G_2 7
#define Gs_2 8
#define A_2 9
#define As_2 10
#define B_2 11
#define C_1 12
#define Cs_1 13
#define D_1 14
#define Ds_1 15
#define E_1 16
#define F_1 17
#define Fs_1 18
#define G_1 19
#define Gs_1 20
#define A_1 21
#define As_1 22
#define B_1 23
#define C0 24
#define Cs0 25
#define D0 26
#define Ds0 27
#define E0 28
#define F0 29
#define Fs0 30
#define G0 31
#define Gs0 32
#define nA0 33
#define nAs0 34
#define B0 35
#define C1 36
#define Cs1 37
#define D1 38
#define Ds1 39
#define E1 40
#define F1 41
#define Fs1 42
#define G1 43
#define Gs1 44
#define nA1 45
#define nAs1 46
#define B1 47
#define C2 48
#define Cs2 49
#define D2 50
#define Ds2 51
#define E2 52
#define F2 53
#define Fs2 54
#define G2 55
#define Gs2 56
#define nA2 57
#define nAs2 58
#define B2 59
#define C3 60
#define Cs3 61
#define D3 62
#define Ds3 63
#define E3 64
#define F3 65
#define Fs3 66
#define G3 67
#define Gs3 68
#define nA3 69
#define nAs3 70
#define B3 71
#define C4 72
#define Cs4 73
#define D4 74
#define Ds4 75
#define E4 76
#define F4 77
#define Fs4 78
#define G4 79
#define Gs4 80
#define nA4 81
#define nAs4 82
#define B4 83
#define C5 84
#define Cs5 85
#define D5 86
#define Ds5 87
#define E5 88
#define F5 89
#define Fs5 90
#define G5 91
#define Gs5 92
#define nA5 93
#define nAs5 94
#define B5 95
#define C6 96
#define Cs6 97
#define D6 98
#define Ds6 99
#define E6 100
#define F6 101
#define Fs6 102
#define G6 103
#define Gs6 104
#define nA6 105
#define nAs6 106
#define B6 107

//#define DEBUG

const int hitPins[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A2, A0, A1 };
const int numPins = sizeof(hitPins) / sizeof(int);
const int noteNumbers[] = { B2, D3, E3, Fs3, nA3, B3, B3, D4, E4, Fs4, nA4, B4, D5 };

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

    if (states[i] == OFF && digitalRead(hitPins[i]) == LOW) {
      if ((timeNow - lastHits[i]) > NOTE_DURATION) {
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

