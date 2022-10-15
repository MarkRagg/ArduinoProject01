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

#define CORRECT 1
#define INCORRECT 0 

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

int leds[4] = {LED_PIN1, LED_PIN2, LED_PIN3, LED_PIN4};
int buttons[4] = {BUTTON_PIN1, BUTTON_PIN2, BUTTON_PIN3, BUTTON_PIN4};

Timer timer(MILLIS);
int patternLeds[4];
int penalty;
long prevts;

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
  for(int i = 0; i < GAME_LEDS; i++) {
    pinMode(leds[i], OUTPUT);
    pinMode(buttons[i], INPUT);
    patternLeds[i] = INCORRECT;
  }
  pinMode(POTENZIOMETRO, INPUT);
  pinMode(LED_PIN_ROSSO, OUTPUT);
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
  int correctLeds = 0;

  setLedsState(LOW);
  Serial.println(timer_start);
  delay(timer_start * 1000);

  ledsOn = createPattern();
  timer.start();
  while(timer.read() <= timer_for_click) {
    for(int i = 0; i < GAME_LEDS; i++){
      if(isButtonPressed(i)){
        addPenalty("PENALTY: TOO EARLY");
        turn_lost = 1;
      }
    }
  }

  timer.stop();

  //delay(timer_for_click);
  setLedsState(LOW);
  timer.start();

  while(timer.read() < timer_for_click || turn_won) {
        
    if (turn_lost) {
      break;
    }

    if (ledsOn == correctLeds) {
      turn_won = 1;
      score += 1;
      timer.pause();
      Serial.println("New point! Score: ");
      Serial.println(score);
      break;
    }
    
    for(int i = 0; i < GAME_LEDS; i++) {
      if(isButtonPressed(i)) {
          if(patternLeds[i] == CORRECT) {
            digitalWrite(leds[i], HIGH);
            patternLeds[i] = 2;
            correctLeds++;
        } else if (patternLeds[i] == INCORRECT) {
            addPenalty("PENALTY: WRONG PATTERN");
            timer.pause();
            turn_lost = 1;
            break;
        }
      }
    }
  }

  delay(1000);
  setLedsState(LOW);
  
  if(timer.read() >= timer_for_click) {
    addPenalty("PENALTY: TIME OVER");
  }

  return;
}

void setLedsState(int state) {
  for(int i = 0; i < GAME_LEDS; i++) {
    digitalWrite(leds[i], state);
  }
}

int createPattern() {
  int ledState = 0;
  int ledsOn = 0;

  for(int i = 0; i < GAME_LEDS; i++) {
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

bool isButtonPressed(int button){
  if(digitalRead(button+2) == HIGH){
    return true;
  }
  return false;
}
