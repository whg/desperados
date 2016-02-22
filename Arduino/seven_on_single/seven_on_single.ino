volatile uint16_t counter = 0;
volatile uint16_t secondsElapsed = 0, frameCount = 0, c2 = 0;
#include <TimerOne.h>
#include "data.h"

#define MAX_ANALOG_VAL 1013

volatile uint16_t globalTime = 700;

const uint16_t NUM_COLS = 4;
const uint16_t NUM_ROWS = 1;
const uint16_t NUM_DIGITS = NUM_COLS * NUM_ROWS;
const uint16_t FRAME_LENGTH = 40; //in millis
const uint8_t  NUM_FRAMES_IN_SEC = 1; //1000 / FRAME_LENGTH;
const uint16_t COUNTER_REDUCTION = FRAME_LENGTH / NUM_COLS * NUM_COLS;
#define INDEX(row, col) ((row) * NUM_COLS + (col))


uint8_t numbers[] = { 63, 6, 91, 79, 102, 109, 125, 7, 127, 111, 63 };
uint8_t tbtNumbers[][9] = {
  { 78, 35, 124, 120, 0, 199, 231, 156, 113 },
  { 66, 109, 0, 0, 109, 0, 136, 237, 136 },
  { 110, 35, 124, 0, 140, 49, 222, 168, 206 },
  { 71, 35, 108, 0, 98, 56, 196, 156, 83 },
  { 0, 218, 16, 222, 138, 152, 0, 158, 156 },
  { 79, 1, 1, 67, 35, 236, 212, 156, 115 },
  { 132, 99, 33, 125, 98, 252, 229, 156, 83 },
  { 113, 1, 123, 0, 134, 0, 0, 113, 0 },
  { 78, 35, 108, 15, 98, 56, 229, 156, 115 },
  { 90, 35, 108, 103, 84, 111, 140, 92, 32 },
};
uint8_t segs[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

uint8_t six[] = { 132, 99, 33, 125, 98, 252, 229, 156, 83 };
int clocks[] = { 8, 9, 10 };
int clearPin = A4;
int shift = 0;
char *message = "HELO";

uint8_t meterInPin = A6;
uint16_t analogVal;

#define NLEDS 2
#define LED_ON_TIME 100

uint8_t ledPins[NLEDS] = { 9, 10 };
uint8_t ledStates[NLEDS] = { LOW, LOW };
long ledOnTimes[NLEDS];

void mainTimer();

const uint8_t NUM_BUFFERS = 1;
uint8_t frameBuffers[NUM_BUFFERS][NUM_COLS * NUM_ROWS];
uint8_t currentFrame[NUM_COLS * NUM_ROWS];
uint8_t fbc = 0; // frame buffer counter
inline uint8_t* nextBuffer() {
  fbc = ++fbc % NUM_BUFFERS;
  return frameBuffers[fbc];
}


// show a 4 digit time in the central group of 4
inline void centralTime(uint16_t time) {

  uint8_t *frameBuffer = currentFrame;
  for (uint8_t i = 0; i < NUM_DIGITS; frameBuffer[i] = 0, i++);

  for (uint8_t i = 0; i < 4; i++) {
    uint8_t d = time % 10;
    time /= 10;
    frameBuffer[3-i] = numbers[d];
  }
}


void setup() {

  for (int i = 0; i < 14; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  for (int i = 0; i < NUM_ROWS; i++) {
    pinMode(clocks[i], OUTPUT);
    digitalWrite(clocks[i], LOW);
    digitalWrite(clocks[i], HIGH);
  }

  PORTC = 0;
  PORTD = 0;
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);


  Timer1.initialize(1000);
  Timer1.attachInterrupt(mainTimer);

  pinMode(clearPin, OUTPUT);
  digitalWrite(clearPin, LOW);


  pinMode(meterInPin, INPUT);
  randomSeed(analogRead(meterInPin));
}

void loop() {
  long timeNow = millis();
  
  for (int i = 0; i < NLEDS; i++) {
    if (random(1000) < 2) { 
      digitalWrite(ledPins[i], HIGH);
      ledStates[i] = HIGH;  
      ledOnTimes[i] = timeNow;
    }
    if (ledStates[i] == HIGH && (timeNow - ledOnTimes[i]) > LED_ON_TIME) {
      digitalWrite(ledPins[i], LOW);
      ledStates[i] = LOW;  
    }
  }
  
  analogVal = map(analogRead(meterInPin), 0, MAX_ANALOG_VAL, 0, 7283);
}

void lightDigits(uint8_t col) {

  // turn everything off first
  PORTC = 15;

  int val = 0;
  for (int row = 0; row < NUM_ROWS; row++) {

    digitalWrite(clocks[row], LOW);
    int k = INDEX(row, col);

    val = currentFrame[k];


    PORTD = (val & 255);
    digitalWrite(clocks[row], HIGH);
  }


  PORTC = 15 ^ (1 << col);

}


byte thirteenhigh = 0;
byte rising = true;


float easeInEaseOut(float t, float b, float c, float d) {
  t /= d / 2;
  if (t < 1) return c / 2 * t * t + b;
  t--;
  return -c / 2 * (t * (t - 2) - 1) + b;
};

float easingCounter = 0;

void mainTimer() {
  if (!(counter % FRAME_LENGTH)) {

    //       int value;
    if (rising) {
      c2 += 3000;
      //          value = c2;

    }
    else {
      //          c2-=17;
      c2 = easeInEaseOut(easingCounter, 7642, -7600, 1);
      easingCounter += 0.2;
    }

    if (c2 > 7642) {
      rising = false;

    }
    else if (c2 < 200) {
      rising = true;
      easingCounter = 0;
    }


    //        interpolateFrames();
    frameCount++;
    counter = counter % NUM_COLS;

    if (!(frameCount % NUM_FRAMES_IN_SEC)) {
//      centralTime(8888);
      centralTime(analogVal);
    }

  }
  
  lightDigits((counter++) % NUM_COLS);
}

