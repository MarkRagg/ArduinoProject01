#include "lib.h"
#include "Timer.h"
#include <avr/sleep.h>
#include "EnableInterrupt.h"

// Variables to manage the fading
int fadeAmount;
int currIntensity;

// Variable that describe the current state of the game
int state;

// Variables to calculate the difficulty
int difficulty;
double incDiff;

// Variable that contains the current score of the player
unsigned int score;

int leds[GAME_LEDS] = { LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4 };
int buttons[GAME_BUTTONS] = { BUTTON_PIN1, BUTTON_PIN2, BUTTON_PIN3, BUTTON_PIN4 };

// Array that tells which leds are in the current pattern
int patternLeds[4];

// Variable that contains the current number of penalties
int penalties;

// Variable for the timer
Timer timer(MILLIS);

int initialWaitingTime;

// Variable that contains the viewing time of the pattern
double patternTime;

// Variable that contains the available time to press the buttons
double availableTime;

// Variable that tells the state of the current turn
int turnLost;

// Variable that contains the number of leds on
int ledsOn;

// Variable that contains the number of correct leds choosen by the player
int correctLeds;

void initialize() {
  Serial.begin(9600);

  initializePins();
  initializeVariables();
  setInterrupts();

	Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");

  // Start the timer for the sleep
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

	// Checks if it is time to go to sleep
	if (timer.read() >= SEC_TO_SLEEP) {
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

  // Checks if the game is over
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

  // Starts the viewing time of the pattern
  timer.start();

  // Checks if the player press a button too early
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
  // Starts the available time to press the buttons
  timer.start();

  while (timer.read() <= availableTime && !turnLost) {

    // Checks if the player reproduced the pattern
    if (ledsOn == correctLeds) {
      score++;
      timer.pause();
      Serial.print("\nNew point! Score: ");
      Serial.println(score);

      /* 
        The time available decreases, if the player scored, up to the minimum time,
        that is different for every difficulty
      */
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

    // Checks the button pressed by the player
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

  // Checks if the player exceeded the time available
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
