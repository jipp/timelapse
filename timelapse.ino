#include <Arduino.h>

#include <Bounce2.h>
#include <EEPROM.h>

// 7 segments
const byte a = 0;
const byte b = 1;
const byte c = 2;
const byte d = 3;
const byte e = 4;
const byte f = 5;
const byte g = 6;
const byte segment[] = {a, b, c, d, e, f, g};

// button for program select
const byte button = 7;

// ASLR focus and shutter
const byte focus = 9;
const byte shutter = 10;

// program
const byte address = 0;

// duration of 7 segment light on
const byte ledOn = 5;

bool buttonState = false;
byte program = 0;
bool pressed = false;
bool ledState = false;

unsigned long ledShine = 0L;
unsigned long shootTime = 0L;

// program -> length
byte programs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const byte programLimit = sizeof(programs) / sizeof(programs[0]) - 1;

Bounce debouncer = Bounce();

bool sevenSeg[][7] = {
  { 0, 1, 1, 0, 0, 0, 0 }, // = 1
  { 1, 1, 0, 1, 1, 0, 1 }, // = 2
  { 1, 1, 1, 1, 0, 0, 1 }, // = 3
  { 0, 1, 1, 0, 0, 1, 1 }, // = 4
  { 1, 0, 1, 1, 0, 1, 1 }, // = 5
  { 1, 0, 1, 1, 1, 1, 1 }, // = 6
  { 1, 1, 1, 0, 0, 0, 0 }, // = 7
  { 1, 1, 1, 1, 1, 1, 1 }, // = 8
  { 1, 1, 1, 0, 0, 1, 1 }, // = 9
  { 1, 1, 1, 1, 1, 1, 0 }, // = 0
  { 0, 0, 0, 0, 0, 0, 0 }  // = off
};

void setup() {
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(focus, OUTPUT);
  pinMode(shutter, OUTPUT);

  pinMode(button, INPUT);

  debouncer.attach(button);
  debouncer.interval(5);

  program = EEPROM.read(address);

  sevenSegWrite(10);
  digitalWrite(focus, LOW);
  digitalWrite(shutter, LOW);
}

void loop() {
  debouncer.update();
  buttonState = debouncer.read();

  if (ledState && buttonState && !pressed) {
    if (program++ > programLimit) {
      program = 0;
    }
    pressed = true;
    EEPROM.write(address, program);
    ledShine = millis();
    sevenSegWrite(program);
  }
  if (ledState && pressed && !buttonState) {
    pressed = false;
  }
  if (!ledState && !pressed && buttonState) {
    ledState = true;
    pressed = true;
    ledShine = millis();
    sevenSegWrite(program);
  }
  if (ledState && (millis() > ledShine + ledOn * 1000)) {
    ledState = false;
    pressed = false;
    sevenSegWrite(10);
  }
  if (millis() > shootTime + programs[program] * 1000) {
    shootTime = millis();
    shoot();
  }
}

void shoot() {
    digitalWrite(focus, HIGH);
    delay(100);
    digitalWrite(shutter, HIGH);
    delay(250);
    digitalWrite(shutter, LOW);
    digitalWrite(focus, LOW);
}

void sevenSegWrite(byte digit) {
  for (byte i = 0; i < 7; i++) {
    digitalWrite(segment[i], 1 - sevenSeg[digit][i]);
  }
}
