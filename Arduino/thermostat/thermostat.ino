#define CONTROL_CHANGE 0b10110000

const int thermostatPins[4] = { 4, 5, 6, 7 };
const int numPins = sizeof(thermostatPins) / sizeof(int);
const int thermostatControl = 90;
const int thermostatValue[] = { 90, 91, 92, 93 };

const int led1Pin = A0;
const int led2Pin = A1;

int currentThemostatState = 0;

void setup() {
  Serial.begin(31250);
  
  for (int i = 0; i < numPins; i++) {
    pinMode(thermostatPins[i], INPUT_PULLUP);
  }

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
}

void loop() {
  for (int i = 0; i < numPins; i++) {
    if (digitalRead(thermostatPins[i]) == LOW) {
      if (currentThemostatState != i) {
        sendMidi(CONTROL_CHANGE, 1, thermostatControl, thermostatValue[i]);
        currentThemostatState = i;

        flashLights();      
      }
    }
  }

  
}

void flashLights() {
  digitalWrite(led1Pin, HIGH);
  delay(100);
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, HIGH);
  delay(300);
  digitalWrite(led2Pin, LOW);
}

void sendMidi(uint8_t type, uint8_t channel, uint8_t byte1, uint8_t byte2) {
  byte cmd = type | ((channel-1) & 15);
  Serial.write(cmd);
  Serial.write(byte1 & 127);
  Serial.write(byte2 & 127);
}
