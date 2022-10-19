/*
Authors:
Dilaver Shtini,     dilaver.shiti@studio.unibo.it,       0000
Francesco Carlucci, francesco.carlucci6@studio.unibo.it, 0000977003
Marco Raggini,      marco.raggini2@studio.unibo.it,      0000
*/

#include "lib.h"

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
        difficulty = (analogRead(POTENTIOMETER) / 256) + 1;
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
        fading();
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


void startGame(int difficulty) {
  int initialWaitingTime = random(1, 6) * 1000;
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
        timer.stop();
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

    for (int i = 0; i < GAME_LEDS; i++) {
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

