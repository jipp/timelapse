#include <Arduino.h>

#include <EEPROM.h>

// 7 segments -> pin#
#define a 0
#define b 1
#define c 2
#define d 3
#define e 4
#define f 5
#define g 6

// button for program select
#define button 7

// ASLR focus and shutter
#define focus 8
#define shutter 9

// store program in EEPROM
const byte address = 0;

// duration of 7 segment light on
const byte ledOn = 5;

// initialize variable
bool buttonState = false;
byte program = 0;
bool pressed = false;
bool ledState = false;
unsigned long ledShine = 0L;
unsigned long shootTime = 0L;

// program# -> length
byte programs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// # of programs
const byte programLimit = sizeof(programs) / sizeof(programs[0]) - 1;

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

  program = EEPROM.read(address);

  // clean display
  sevenSegWrite(10);
  digitalWrite(focus, LOW);
  digitalWrite(shutter, LOW);
}

void loop() {
  buttonState = digitalRead(button);

  if (ledState && buttonState && !pressed) {
    if (program++ >= programLimit) {
      program = 0;
    }
    ledShine = millis();
    pressed = true;
    EEPROM.write(address, program);
    sevenSegWrite(program);
    delay(100);
  }
  if (ledState && pressed && !buttonState) {
    pressed = false;
  }
  if (!ledState && !pressed && buttonState) {
    ledShine = millis();
    ledState = true;
    pressed = true;
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

// either high memory or more program lines - you pay for one with the other
void sevenSegWrite(byte digit) {
  switch (digit) {
    case 1:
      digitalWrite(a, true);
      digitalWrite(b, false);
      digitalWrite(c, false);
      digitalWrite(d, true);
      digitalWrite(e, true);
      digitalWrite(f, true);
      digitalWrite(g, true);
      break;
    case 2:
      digitalWrite(a, false);
      digitalWrite(b, false);
      digitalWrite(c, true);
      digitalWrite(d, false);
      digitalWrite(e, false);
      digitalWrite(f, true);
      digitalWrite(g, false);
      break;
    case 3:
      digitalWrite(a, false);
      digitalWrite(b, false);
      digitalWrite(c, false);
      digitalWrite(d, false);
      digitalWrite(e, true);
      digitalWrite(f, true);
      digitalWrite(g, false);
      break;
    case 4:
      digitalWrite(a, true);
      digitalWrite(b, false);
      digitalWrite(c, false);
      digitalWrite(d, true);
      digitalWrite(e, true);
      digitalWrite(f, false);
      digitalWrite(g, false);
      break;
    case 5:
      digitalWrite(a, false);
      digitalWrite(b, true);
      digitalWrite(c, false);
      digitalWrite(d, false);
      digitalWrite(e, true);
      digitalWrite(f, false);
      digitalWrite(g, false);
      break;
    case 6:
      digitalWrite(a, false);
      digitalWrite(b, true);
      digitalWrite(c, false);
      digitalWrite(d, false);
      digitalWrite(e, false);
      digitalWrite(f, false);
      digitalWrite(g, false);
      break;
    case 7:
      digitalWrite(a, false);
      digitalWrite(b, false);
      digitalWrite(c, false);
      digitalWrite(d, true);
      digitalWrite(e, true);
      digitalWrite(f, true);
      digitalWrite(g, true);
      break;
    case 8:
      digitalWrite(a, false);
      digitalWrite(b, false);
      digitalWrite(c, false);
      digitalWrite(d, false);
      digitalWrite(e, false);
      digitalWrite(f, false);
      digitalWrite(g, false);
      break;
    case 9:
      digitalWrite(a, false);
      digitalWrite(b, false);
      digitalWrite(c, false);
      digitalWrite(d, true);
      digitalWrite(e, true);
      digitalWrite(f, false);
      digitalWrite(g, false);
      break;
    case 0:
      digitalWrite(a, false);
      digitalWrite(b, false);
      digitalWrite(c, false);
      digitalWrite(d, false);
      digitalWrite(e, false);
      digitalWrite(f, false);
      digitalWrite(g, true);
      break;
    default:
      digitalWrite(a, true);
      digitalWrite(b, true);
      digitalWrite(c, true);
      digitalWrite(d, true);
      digitalWrite(e, true);
      digitalWrite(f, true);
      digitalWrite(g, true);
      break;
  }
}
