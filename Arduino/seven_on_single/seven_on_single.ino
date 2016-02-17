volatile uint16_t counter = 0;
volatile uint16_t secondsElapsed = 0, frameCount = 0, c2 = 0;
#include <TimerOne.h>
#include "data.h"

volatile uint16_t globalTime = 700;

const uint16_t NUM_COLS = 4;
const uint16_t NUM_ROWS = 1;
const uint16_t NUM_DIGITS = NUM_COLS * NUM_ROWS;
const uint16_t FRAME_LENGTH = 40; //in millis
const uint8_t  NUM_FRAMES_IN_SEC = 40 / FRAME_LENGTH;
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
    frameBuffer[i] = numbers[d];
  }
  //    frameBuffer[3] = numbers[2];

  //    if ((frameCount / 5) % 2) frameBuffer[17] |= 128;
}

inline void allTime(uint16_t time) {

  uint8_t *frameBuffer = nextBuffer();

  for (uint8_t i = 0; i < 4; i++) {
    uint8_t d = time % 10;
    time /= 10;
    for (uint8_t j = 0; j < 9; j++) {
      frameBuffer[j * 4 + 3 - i] = numbers[d];
    }
  }

  for (uint8_t j = 0; j < 9; j++) {
    frameBuffer[j * 4 + 1] |= 128;
  }

}

inline void singleDigitBigTime(uint16_t time) {
  uint8_t *frameBuffer = nextBuffer();

  uint8_t k, v;
  for (int8_t i = 3; i >= 0; i--) {
    v = time % 10;
    time /= 10;

    for (uint8_t r = 0; r < 3; r++) {
      for (uint8_t c = 0; c < 3; c++) {
        k = INDEX(r, c + i * 3);
        frameBuffer[k] = numbers[v];
      }
    }
  }
}

inline uint8_t numberIndex(uint8_t row, uint8_t col) {
  return col % 3 + row * 3;
}

inline void bigTime(uint16_t time) {

  uint8_t *frameBuffer = nextBuffer();

  uint8_t k, v;
  for (int8_t i = 3; i >= 0; i--) {
    v = time % 10;
    time /= 10;

    for (uint8_t r = 0; r < 3; r++) {
      for (uint8_t c = 0; c < 3; c++) {
        k = INDEX(r, c + i * 3);
        frameBuffer[k] = characters[v][r * 3 + c];
      }
    }
  }
}

inline void oldDigitsTime(uint16_t time) {


  uint8_t *frameBuffer = nextBuffer();

  uint8_t k, v;
  for (int8_t i = 3; i >= 0; i--) {
    v = time % 10;
    time /= 10;

    for (uint8_t r = 0; r < 3; r++) {
      for (uint8_t c = 0; c < 3; c++) {
        k = INDEX(r, c + i * 3);
        frameBuffer[k] = tbtNumbers[v][r * 3 + c];
      }
    }
  }
}

inline void allEights(uint16_t _) {

  uint8_t *frameBuffer = nextBuffer();
  for (uint8_t i = 0; i < NUM_DIGITS; frameBuffer[i] = 255, i++);
}

inline void timeUnits(uint16_t t) {

  uint8_t *frameBuffer = nextBuffer();
  uint8_t units = numbers[t % 10];
  for (uint8_t i = 0; i < NUM_DIGITS; frameBuffer[i] = units, i++);
}

typedef void (*displayFunction)(uint16_t);


volatile displayFunction displayFuncs[] = {
  centralTime,
  //    /* allEights, */
  //    allTime,
  //    singleDigitBigTime,
  //    bigTime,
  //    centralTime,
  //    timeUnits,
  //    bigTime,
  //    oldDigitsTime,
  //    bigTime,
  //    oldDigitsTime,
  //    bigTime,
  //    oldDigitsTime,

};
const uint8_t NUM_DISPLAY_FUNCS = 1;

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


//  Timer1.initialize(1000);
//  Timer1.attachInterrupt(mainTimer);

  pinMode(clearPin, OUTPUT);
  digitalWrite(clearPin, LOW);


  pinMode(A5, INPUT);
  Serial.begin(9600);
}
uint16_t analogVal;
void loop() {

  analogVal = analogRead(A5);
  Serial.println(analogVal);
  delay(50); 
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


  PORTC = 15 ^ (1 << (3 - col));

}


volatile uint8_t interpolationOrders[][8] = {
  { 2, 4, 1, 5, 6, 7, 3, 0 },
  { 0, 3, 7, 6, 5, 4, 2, 1 },
  { 1, 0, 7, 6, 5, 4, 3, 2 },
  { 0, 5, 4, 1, 2, 6, 7, 3 },
  { 1, 7, 6, 0, 3, 4, 5, 2 },
  { 3, 0, 7, 5, 4, 6, 2, 1 },
  { 4, 7, 1, 6, 3, 5, 2, 0 },
  { 6, 5, 1, 3, 4, 7, 0, 2 },
  { 2, 3, 1, 5, 6, 0, 7, 4 },
  { 5, 2, 0, 7, 1, 3, 4, 6 },
  { 6, 1, 0, 2, 5, 7, 3, 4 },
  { 4, 5, 7, 3, 0, 1, 6, 2 },
};

const uint8_t NUM_ORDERS = 12;
volatile uint8_t interpolationOrderCounter = 0;

volatile uint8_t interpolationCounter = 0;


volatile uint8_t dfc = 0;
volatile displayFunction df;

void secondCallback() {


  interpolationCounter = 0;
  interpolationOrderCounter = ++interpolationOrderCounter % NUM_ORDERS;

  df = displayFuncs[dfc];


  df(globalTime);
  globalTime++;
  /* displayFuncs[dfc](6789); */

  // 0123456789
  // 1234 0827

  dfc = ++dfc % NUM_DISPLAY_FUNCS;
}



void interpolateFrames() {

  /* static uint8_t c = 0; */
  uint8_t c = interpolationOrders[interpolationOrderCounter][interpolationCounter];

  uint8_t nfbc = (fbc + 1) % 2;

  //    if (interpolationCounter < 8) {
  //        for (uint8_t i = 0; i < NUM_DIGITS; i++) {
  //            uint8_t segon = frameBuffers[fbc][i] & (1 << c);
  //            currentFrame[i] ^= (-segon ^ currentFrame[i]) & (1 << c);
  //        }
  //        interpolationCounter++;
  //    }
  //    else {
  for (uint8_t i = 0; i < NUM_DIGITS; i++) {
    currentFrame[i] = frameBuffers[fbc][i];
  }
  //    }
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
      centralTime(analogVal);
    }

  }


  //    static int count = 0;
  //    PORTD = numbers[(c2/10)%9];
  //    PORTC = 0;
  //
  //    count = ++count % 9;


  lightDigits((counter++) % NUM_COLS);
}

