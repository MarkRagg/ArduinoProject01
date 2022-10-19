#include "lib.h"
#include <avr/sleep.h>
#include "Timer.h"
#include "EnableInterrupt.h"

int fadeAmount;
int currIntensity;
int state;

/* variables to calculate the difficulty */
int difficulty;
double incDiff;

unsigned int score;

int leds[GAME_LEDS] = { LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4 };
int buttons[GAME_BUTTONS] = { BUTTON_PIN1, BUTTON_PIN2, BUTTON_PIN3, BUTTON_PIN4 };

Timer timer(MILLIS);
int patternLeds[4];
int penalty;


void initialize() {
  for (int i = 0; i < GAME_LEDS; i++) {
    pinMode(leds[i], OUTPUT);
    pinMode(buttons[i], INPUT);
    patternLeds[i] = INCORRECT;
  }
  pinMode(POTENTIOMETER, INPUT);
  pinMode(LED_PIN_ROSSO, OUTPUT);
}

void sleep() {
  Serial.flush();
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

void wakeUp() {
  sleep_disable();
}

void fading() {
  analogWrite(LED_PIN_ROSSO, currIntensity);
  currIntensity = currIntensity + fadeAmount;
  if (currIntensity <= 0 || currIntensity >= 255) {
    fadeAmount = -fadeAmount;
  }
}

void setLedsState(int state) {
  for (int i = 0; i < GAME_LEDS; i++) {
    digitalWrite(leds[i], state);
  }
}

bool isButtonPressed(int button) {
  if (digitalRead(button + 2) == HIGH) {
    return true;
  }
  return false;
}

void addPenalty(String msg) {
  penalty++;
  Serial.println(msg);
  digitalWrite(LED_PIN_ROSSO, HIGH);
  delay(1000);
}

int createPattern() {
  int ledState = 0;
  int ledsOn = 0;

  for (int i = 0; i < GAME_LEDS; i++) {
    ledState = random(0, 2);
    digitalWrite(leds[i], ledState);
    patternLeds[i] = ledState;

    if (ledState == HIGH) {
      ledsOn++;
    }
  }
  return ledsOn;
}