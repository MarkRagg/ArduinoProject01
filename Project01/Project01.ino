#define LED_PIN_ROSSO 9
#define LED_PIN1 10
#define LED_PIN2 11
#define LED_PIN3 12
#define LED_PIN4 13
#define BUTTON_PIN1 2
#define BUTTON_PIN2 3
#define BUTTON_PIN3 4
#define BUTTON_PIN4 5
#define POTENZIOMETRO A0

#define START_GAME_BUTTON 0
#define GAME_LEDS 4

#define INCORRECT 0
#define CORRECT 1
#define TAKEN 2

#define INITIAL_STATE 100
#define IN_GAME 101

#define MAX_PENALTIES 3

#include <avr/sleep.h>
#include "Timer.h"
#include "EnableInterrupt.h"

int fadeAmount;
int currIntensity;
int state;

/** variables to calculate the difficulty */
int difficulty;
double incDiff;

unsigned int score;

int leds[4] = { LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4 };
int buttons[4] = { BUTTON_PIN1, BUTTON_PIN2, BUTTON_PIN3, BUTTON_PIN4 };

Timer timer(MILLIS);
int patternLeds[4];
int penalty;

void wakeUp() {
  sleep_disable();
}

void setup() {
  Serial.begin(9600);
  initialize();
  currIntensity = 0;
  fadeAmount = 1;
  state = INITIAL_STATE;
  penalty = 0;
  score = 0;
  randomSeed(analogRead(5));

  enableInterrupt(BUTTON_PIN1, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN2, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN3, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN4, wakeUp, RISING);
  Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");
  timer.start();
}

void loop() {
  delay(5);  //delay per fading

  switch (state) {
    case INITIAL_STATE:
      if (isButtonPressed(START_GAME_BUTTON)) {
        difficulty = (analogRead(POTENZIOMETRO) / 256) + 1;
        score = 0;
        incDiff = 0.25;
        penalty = 0;
        Serial.println("GO!\n");
        Serial.print("Difficulty: ");
        Serial.println(difficulty);

        digitalWrite(LED_PIN_ROSSO, LOW);
        state = IN_GAME;
        startGame(difficulty);
      } else {
        currIntensity += fadeAmount;
        if (currIntensity <= 0 || currIntensity >= 255) {
          fadeAmount = -fadeAmount;
        }
        analogWrite(LED_PIN_ROSSO, currIntensity);
      }

      if (timer.read() >= 10000) {
        timer.stop();
        Serial.println("Sleep mode: On");
        digitalWrite(LED_PIN_ROSSO, LOW);
        sleep();
        Serial.println("Sleep mode: Off");
        delay(300);
        timer.start();
      }
      break;
      
    case IN_GAME:
      digitalWrite(LED_PIN_ROSSO, LOW);
      if (penalty >= MAX_PENALTIES) {
        Serial.println("\nGAME OVER!");
        Serial.print("Final Score: ");
        Serial.println(score);

        state = INITIAL_STATE;
        delay(10000);
        timer.start();
        Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");
      } else {
        startGame(difficulty);
      }
      break;
  }
}

void initialize() {
  for (int i = 0; i < GAME_LEDS; i++) {
    pinMode(leds[i], OUTPUT);
    pinMode(buttons[i], INPUT);
    patternLeds[i] = INCORRECT;
  }
  pinMode(POTENZIOMETRO, INPUT);
  pinMode(LED_PIN_ROSSO, OUTPUT);
}

void sleep() {
  Serial.flush();
  delay(100);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

void startGame(int difficulty) {
  int initialWaitingTime = random(1, 6) * 1000;
  Serial.println(incDiff); 
  double patternTime = (10 - ((double)difficulty * incDiff)) * 1000;
  double availableTime = patternTime;
  int turnLost = 0;
  int ledsOn = 0;
  int correctLeds = 0;
  double var = 0.25;

  setLedsState(LOW);

  Serial.print("\nSeconds to recreate the pattern: ");
  Serial.print(availableTime / 1000);

  delay(initialWaitingTime);

  ledsOn = createPattern();
  timer.start();

  while (timer.read() <= patternTime && !turnLost) {
    for (int i = 0; i < GAME_LEDS; i++) {
      if (isButtonPressed(i)) {
        addPenalty("\nPENALTY: TOO EARLY");
        turnLost = 1;
        break;
      }
    }
  }

  timer.stop();

  setLedsState(LOW);
  timer.start();

  while (timer.read() <= availableTime && !turnLost) {
    if (ledsOn == correctLeds) {
      score++;
      timer.pause();
      Serial.print("\nNew point! Score: ");
      Serial.println(score);
      Serial.println(availableTime);
      switch (difficulty) {
        case 1:
          incDiff = (availableTime <= 2000 ? incDiff = incDiff : incDiff + 0.25);
          break;
        case 2:
          incDiff = (availableTime <= 1500 ? incDiff = incDiff : incDiff + 0.25);
          break;
        case 3:
          incDiff = (availableTime <= 1250 ? incDiff = incDiff : incDiff + 0.25);
          break;
        case 4:
          incDiff = (availableTime <= 1000 ? incDiff = incDiff : incDiff + 0.25);
          break;
      }
      break;
    }

    for (int i = 0; i < GAME_LEDS; i++) {
      if (isButtonPressed(i)) {
        if (patternLeds[i] == CORRECT) {
          digitalWrite(leds[i], HIGH);
          patternLeds[i] = TAKEN;
          correctLeds++;
        } else if (patternLeds[i] == INCORRECT) {
          addPenalty("\nPENALTY: WRONG PATTERN");
          timer.pause();
          turnLost = 1;
          break;
        }
      }
    }
  }

  delay(1000);
  setLedsState(LOW);

  if (timer.read() >= availableTime) {
    addPenalty("\nPENALTY: TIME OVER");
  }
  return;
}

void setLedsState(int state) {
  for (int i = 0; i < GAME_LEDS; i++) {
    digitalWrite(leds[i], state);
  }
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

void addPenalty(String msg) {
  penalty++;
  Serial.println(msg);
  digitalWrite(LED_PIN_ROSSO, HIGH);
  delay(1000);
}

bool isButtonPressed(int button) {
  if (digitalRead(button + 2) == HIGH) {
    return true;
  }
  return false;
}
