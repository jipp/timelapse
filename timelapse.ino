#include <Bounce2.h>
#include <EEPROM.h>

const int button = 7;
const int led = 8;

const int address = 0;
const int ledOn = 5000;

int buttonState = 0;
byte program = 0;
bool pressed = false;
bool ledState = false;

unsigned long ledShine = 0L;
unsigned long shootTime = 0L;

int programs[] = {1000, 2000, 3000, 5000, 8000, 13000, 21000, 34000, 55000, 89000};
const int programLimit = sizeof(programs) / sizeof(programs[0]) - 1;

Bounce debouncer = Bounce();

byte sevenSeg[11][7] = {
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
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(led, OUTPUT);

  pinMode(button, INPUT);

  debouncer.attach(button);
  debouncer.interval(5);

  program = EEPROM.read(address);

  sevenSegWrite(10);
  digitalWrite(led, LOW);
}

void loop() {
  debouncer.update();
  buttonState = debouncer.read();

  if (ledState && buttonState == HIGH && !pressed) {
    program ++;
    if (program > programLimit) {
      program = 0;
    }
    pressed = true;
    EEPROM.write(address, program);
    sevenSegWrite(program);
    ledShine = millis();
  }
  if (ledState && pressed && buttonState == LOW) {
    pressed = false;
  }
  if (!ledState && !pressed && buttonState == HIGH) {
    ledState = true;
    pressed = true;
    sevenSegWrite(program);
    ledShine = millis();
  }
  if (ledState && (millis() > ledShine + ledOn)) {
    ledState = false;
    pressed = false;
    sevenSegWrite(10);
  }
  if (millis() > shootTime + programs[program]) {
    digitalWrite(led, HIGH);
    delay(250);
    digitalWrite(led, LOW);
    shootTime = millis();
  }
}

void sevenSegWrite(byte digit) {
  for (byte i = 0; i < 7; i++) {
    digitalWrite(i, 1 - sevenSeg[digit][i]);
  }
}
