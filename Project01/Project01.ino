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

#define INITIAL_STATE 100
#define IN_GAME 101

#define MAX_PENALTIES 3 

#include <avr/sleep.h>
#include "Timer.h"

int pressed;
int brightness;
int fadeAmount;
int currIntensity;
int state;
int difficulty;
unsigned int score;
int buttonsState[4];

int leds[4] = {LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4};
Timer timer(MILLIS);
int gameLeds[4];
int penalty;
long prevts;

void wakeUp() {
  /** The program will continue from here. **/
  /* First thing to do is disable sleep. */
  sleep_disable();
}


void setup() {
  Serial.begin(9600);
  initialize();
  currIntensity = 0;
  fadeAmount = 1;
  state = INITIAL_STATE;
  penalty = 0;
  prevts = 0;
  score = 0;
  randomSeed(analogRead(5));

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN1), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN2), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN3), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN4), wakeUp, RISING);
  Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");

  timer.start();
}

void loop() {
  int difficulty = analogRead(POTENZIOMETRO) / 256;                 
 
  delay(5);

  switch (state) {
    case INITIAL_STATE :
      if (readButton(0) == HIGH) {
      digitalWrite(LED_PIN_ROSSO, LOW); 
      delay(100);
      state = IN_GAME;
      Serial.println("GO!");
      startGame(difficulty);
    } else {
      currIntensity += fadeAmount;
      if (currIntensity <= 0 || currIntensity >= 255) {
        fadeAmount = -fadeAmount;
      }
      analogWrite(LED_PIN_ROSSO, currIntensity); 
    }

    if(timer.read() >= 10000) {
      timer.stop();
      Serial.println("Sleep mode: On");
      digitalWrite(LED_PIN_ROSSO, LOW); 
      sleep();
      Serial.println("Sleep mode: Off");
      delay(300);
      timer.start();
    }
    break;
    case IN_GAME :
      digitalWrite(LED_PIN_ROSSO, LOW); 
      if(penalty >= MAX_PENALTIES) {
        penalty = 0;
        score = 0;
        Serial.println("GAME OVER!");
        Serial.println("Final Score:");
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
  delay(100);
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
  gameLedsOff();
  Serial.println(timer_start);
  delay(timer_start * 1000);
  ledsOn = randomLedsOn();
  delay(timer_for_click);
  ledsOnOrOff(LOW);
  timer.start();
  while(timer.read() < timer_for_click || turn_won) {
    if (ledsOn == ledsTakes) {
      turn_won = 1;
      score += 1;
      Serial.println("New point! Score: ");
      Serial.println(score);
      break;
    }
    
    for(int i = 0; i < 4; i++) {
      if (readButton(i) == HIGH && gameLeds[i] == 1) {
        digitalWrite(leds[i], HIGH);
        gameLeds[i] = 2;
        ledsTakes++;
      } else if (readButton(i) && gameLeds[i] == 0) {
        penalty++;
        Serial.println("PENALTY: WRONG PATTERN");
        digitalWrite(LED_PIN_ROSSO, HIGH);
        delay(1000);
        timer.pause();
        turn_lost = 1;
        break;
      }
    }
    
    if (turn_lost) {
      break;
    }
  }
  delay(1000);
  ledsOnOrOff(LOW);
  
  if(timer.read() >= timer_for_click) {
    penalty++;
    Serial.println("PENALTY: TIME OVER");
    digitalWrite(LED_PIN_ROSSO, HIGH);
    delay(1000);
  }

  return;
}

void ledsOnOrOff(int type) {
  for(int i = 0; i < 4; i++) {
    digitalWrite(leds[i], type);
  }
}

void gameLedsOff() {
  for(int i = 0; i<4; i++) {
     gameLeds[i] = 0;
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

int readButton(int button){
  if(digitalRead(button+2) == HIGH){
    return HIGH;
  }
  return LOW;
}
