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

/** variables to calculate the difficulty */
int difficulty;
int incDiff;
int availableTime;    /* rappresenta il tempo disponibile in cui rimangono accesi i led durante il gioco E il tempo a disposizione per riprodurre il pattern corretto  */

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
  // Enable PCIE2 Bit3 = 1 (Port D)
  PCICR |= B00000100;
  // Select PCINT20 Bit4 = 1 (Pin D4)
  // Select PCINT21 Bit5 = 1 (Pin D5)
  PCMSK2 |= bit (PCINT20);
  PCMSK2 |= bit (PCINT21);
  Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");
  timer.start();
}

/** AttachInterrupt funziona solo con i pin 2 e 3, per utilizzare altri pin con gli interrupt 
    utilizzo la funzione ISR */
ISR(PCINT2_vect) {
    sleep_disable();
}  

void loop() {
  int difficulty = analogRead(POTENZIOMETRO) / 256;        

  delay(5);

  switch (state) {
    case INITIAL_STATE :
      if (isButtonPressed(START_GAME_BUTTON)) {
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
        Serial.println("GAME OVER!");
        Serial.println("Final Score:");
        Serial.println(score);
        score = 0;
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
  int availableTime = (10 - difficulty - incDiff) * 1000;
  int turn_won = 0;
  int turn_lost = 0;
  int ledsOn = 0;
  int ledsTakes = 0;

  ledsOnOrOff(LOW);
  gameLedsOff();
  Serial.println(availableTime);
  Serial.print("incDiff: ");
  Serial.println(incDiff);
  Serial.print("Difficulty: ");
  Serial.println(difficulty);
  delay(timer_start*1000);
  ledsOn = randomLedsOn();
  delay(availableTime);
  ledsOnOrOff(LOW);
  timer.start();
  while(timer.read() < availableTime || turn_won) {
    if (ledsOn == ledsTakes) {
      turn_won = 1;
      score += 1;
      Serial.println("New point! Score: ");
      Serial.println(score);
      switch(difficulty) {
          case 1:
            incDiff == 8 ? incDiff=incDiff : incDiff++;
            break;
          case 2:
            incDiff == 7 ? incDiff=incDiff : incDiff++;
            break;
          case 3:
            incDiff == 6 ? incDiff=incDiff : incDiff++;
            break;
          case 4:
            incDiff == 5 ? incDiff=incDiff : incDiff++;
            break;
      }
      break;
    }
    
    for(int i = 0; i < 4; i++) {
      if (isButtonPressed(i) && gameLeds[i] == 1) {
        digitalWrite(leds[i], HIGH);
        gameLeds[i] = 2;
        ledsTakes++;
      } else if (isButtonPressed(i) && gameLeds[i] == 0) {
        penalty++;
        Serial.println("PENALTY: WRONG PATTERN");
        incDiff = 0;
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
  
  if(timer.read() >= availableTime) {
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

bool isButtonPressed(int button){
  if(digitalRead(button+2) == HIGH){
    return true;
  }
  return false;
}