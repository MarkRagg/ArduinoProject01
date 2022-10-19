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
int penalties;


int initialWaitingTime;
double patternTime;
double availableTime;
int turnLost = 0;
int ledsOn = 0;
int correctLeds = 0;

void initialize() {
  Serial.begin(9600);

  initializePins();
  initializeVariables();
  setInterrupts();
	Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");
  timer.start();    
}

void initializePins() {
  for (int i = 0; i < GAME_LEDS; i++) {
    pinMode(leds[i], OUTPUT);
    pinMode(buttons[i], INPUT);
    patternLeds[i] = INCORRECT;
  }
  pinMode(POTENTIOMETER, INPUT);
  pinMode(LED_PIN_ROSSO, OUTPUT);
}

void initializeVariables() {
  currIntensity = 0;
  fadeAmount = 1;
  state = INITIAL_STATE;
  penalties = 0;
  score = 0;
  randomSeed(analogRead(5));
}

void setInterrupts() {
  enableInterrupt(BUTTON_PIN1, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN2, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN3, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN4, wakeUp, RISING);
}

void initialState() {
  if (isButtonPressed(START_GAME_BUTTON)) {
		difficulty = (analogRead(POTENTIOMETER) / 256) + 1;
		score = 0;
		incDiff = 0.25;
		penalties = 0;
		Serial.println("GO!\n");
		Serial.print("Difficulty: ");
		Serial.println(difficulty);

		digitalWrite(LED_PIN_ROSSO, LOW);
		state = IN_GAME;
		startGame(difficulty);
	} else {
		fading();
	}

	//
	if (timer.read() >= 10000) {
		timer.stop();
		Serial.println("Sleep mode: On");
		digitalWrite(LED_PIN_ROSSO, LOW);
		sleep();
		Serial.println("Sleep mode: Off");
		delay(300);
		timer.start();
	}
}

void inGame() {
	digitalWrite(LED_PIN_ROSSO, LOW);

	if (penalties >= MAX_PENALTIES) {
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
}

void startGame(int difficulty) {
	initialWaitingTime = random(1, 6) * 1000;
  patternTime = (10 - ((double)difficulty * incDiff)) * 1000;
  availableTime = patternTime;
  turnLost = 0;
  ledsOn = 0;
  correctLeds = 0;

	showPattern();

	play();
}

void showPattern() {
	setLedsState(LOW);

  Serial.print("\nSeconds to recreate the pattern: ");
  Serial.print(availableTime / 1000);

  delay(initialWaitingTime);

  ledsOn = createPattern();
  timer.start();

  while (timer.read() <= patternTime && !turnLost) {
    for (int i = 0; i < GAME_BUTTONS; i++) {
      if (isButtonPressed(i)) {
        timer.stop();
        addPenalty("\nPENALTY: TOO EARLY");
        turnLost = 1;
        break;
      }
    }
  }

  timer.stop();

  setLedsState(LOW);
}

void play() {
	
  
  timer.start();

  while (timer.read() <= availableTime && !turnLost) {
    if (ledsOn == correctLeds) {
      score++;
      timer.pause();
      Serial.print("\nNew point! Score: ");
      Serial.println(score);
      switch (difficulty) {
        case 1:
          incDiff = (availableTime <= 2000 ? incDiff = 9.00 : incDiff + 0.25);
          break;
        case 2:
          incDiff = (availableTime <= 1500 ? incDiff = 4.25 : incDiff + 0.25);
          break;
        case 3:
          incDiff = (availableTime <= 1000 ? incDiff = 3.00 : incDiff + 0.25);
          break;
        case 4:
          incDiff = (availableTime <= 1000 ? incDiff = 2.375 : incDiff + 0.25);
          break;
      }
      break;
    }

    for (int i = 0; i < GAME_BUTTONS; i++) {
      if (isButtonPressed(i)) {
        if (patternLeds[i] == CORRECT) {
          digitalWrite(leds[i], HIGH);
          patternLeds[i] = TAKEN;
          correctLeds++;
        } else if (patternLeds[i] == INCORRECT) {
          timer.stop();
          addPenalty("\nPENALTY: WRONG PATTERN");
          turnLost = 1;
          break;
        }
      }
    }
  }

  timer.pause();
  delay(1000);
  setLedsState(LOW);

  if (timer.read() > availableTime) {
    addPenalty("\nPENALTY: TIME OVER");
  }
  return;
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
  penalties++;
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
