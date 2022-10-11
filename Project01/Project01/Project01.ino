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
#define MAX_PENALTIES 3 

#include <avr/sleep.h>
#include "Timer.h"

int pressed;
int brightness;
int fadeAmount;
int currIntensity;
bool gameStart;
int difficulty;
unsigned int score;
int buttonsState[4];
int leds[4] = {LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4};
Timer timer(MILLIS);
int gameLeds[4];
int penalty;

void wakeUp(){
  /** The program will continue from here. **/
  /* First thing to do is disable sleep. */
  sleep_disable();
}

void setup() {
  Serial.begin(9600);
  initialize();
  currIntensity = 0;
  fadeAmount = 5;
  gameStart = false;
  penalty = 0;
  randomSeed(analogRead(5));
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN1), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN2), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN3), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN4), wakeUp, RISING);
  Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");
  timer.start();
}

void loop() {
  for(int i = 0; i < 4; i++) {
    buttonsState[i] = digitalRead(i+2);
  }

  if(timer.read() >= 10000) {
    timer.stop();
    Serial.println("Sleep mode: On");
    sleep();
    Serial.println("Sleep mode: Off");
    timer.start();
  }
  
  int difficulty = analogRead(POTENZIOMETRO) / 256;                 

  if (buttonsState[0] == HIGH && gameStart == false) {
    gameStart = true;
    currIntensity = 0;
    analogWrite(LED_PIN_ROSSO, currIntensity); 
    startGame(difficulty);
  } else if(gameStart == false) {
    analogWrite(LED_PIN_ROSSO, currIntensity);   
    currIntensity = currIntensity + fadeAmount;
    if (currIntensity == 0 || currIntensity == 255) {
      fadeAmount = -fadeAmount;
    } 
  }
  
}

void initialize(){
  for(int i = 0; i<4; i++) {
    pinMode(leds[i], OUTPUT);
    gameLeds[i] = 0;
  }
  for(int i = 2; i<6; i++) {
    pinMode(i, INPUT);
  }

  pinMode(POTENZIOMETRO, INPUT);
  pinMode(LED_PIN_ROSSO, INPUT);
}

void sleep(){
  Serial.flush();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

void startGame(int difficulty) {  
  long int timer_start = random(0,6);
  int timer_for_click = (5 - difficulty) * 1000;
  int turn_won = 0;
  int turn_lost = 0;
  int ledsOn = 0;
  int ledsTakes = 0;
  ledsOnOrOff(LOW);
  Serial.println(timer_start);
  delay(timer_start * 1000);
  Serial.println("GO!");
  ledsOn = randomLedsOn();
  timer.start();
  while(timer.read() < timer_for_click || turn_won) {
    for(int i = 0; i < 4; i++) {
      buttonsState[i] = digitalRead(i+2);
      Serial.println(buttonsState[0]);
      if (buttonsState[i] && gameLeds[i]) {
        score += 10;
        gameLeds[i] = 0;
        ledsTakes++;
      } else if (buttonsState[i] && gameLeds[i] == 0) {
        penalty++;
        Serial.println("PENALTY: WRONG PATTERN");
        timer.pause();
        turn_lost = 1;
        break;
      }

      if (ledsOn == ledsTakes) {
        turn_won = 1;
        break;
      }
    }
    if (turn_lost) {
      break;
    }
  }
  ledsOnOrOff(LOW);
  
  if(timer.read() >= timer_for_click) {
    penalty++;
    Serial.println("PENALTY: TIME OVER");
  }

  if(penalty >= MAX_PENALTIES) {
    Serial.println("GAME OVER!");
    Serial.println("Score:");
    Serial.println(score);
    return;
  }
  return;
}

void ledsOnOrOff(int type) {
  for(int i = 0; i < 4; i++) {
    digitalWrite(leds[i], type);
    gameLeds[i] = type;
  }
}

int randomLedsOn() {
  int randnum = 0;
  int ledsOn = 0;
  for(int i = 0; i<4; i++) {
    randnum = random(0, 2);
    digitalWrite(leds[i], randnum);
    gameLeds[i] = randnum;
    if (randnum) {
      ledsOn++;
    }
  }

  return ledsOn;
}
