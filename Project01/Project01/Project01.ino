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
Timer timer(MILLIS);

void wakeUp(){
  /** The program will continue from here. **/
  Serial.println("WAKE UP");
  /* First thing to do is disable sleep. */
  sleep_disable();
}

void setup() {
  Serial.begin(9600);
  initialize();
  currIntensity = 0;
  fadeAmount = 5;
  gameStart = false;
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
  Serial.println(timer.read());
  if(timer.read() >= 7000) {
    timer.stop();
    sleep();
    timer.start();
  }
  
  int difficulty = analogRead(POTENZIOMETRO) / 256;                 

  if (buttonsState[0] == HIGH && gameStart == false) {
    startGame(difficulty);
    gameStart = true;
    currIntensity = 0;
    analogWrite(LED_PIN_ROSSO, currIntensity); 
  } else if(gameStart == false) {
    analogWrite(LED_PIN_ROSSO, currIntensity);   
    currIntensity = currIntensity + fadeAmount;
    if (currIntensity == 0 || currIntensity == 255) {
      fadeAmount = -fadeAmount;
    } 
  }
  
  if (buttonsState[0] == HIGH) {
    digitalWrite(LED_PIN1, HIGH);
  } else {
    digitalWrite(LED_PIN1, LOW);
 }
}

void initialize(){
  for(int i = 9; i<14; i++) {
    pinMode(i, OUTPUT);
  }
  for(int i = 2; i<6; i++) {
    pinMode(i, INPUT);
  }

  pinMode(POTENZIOMETRO, INPUT);
}

void sleep(){
  Serial.flush();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

void startGame(int difficulty) {  
  int timer = 5 - difficulty;
  return;
}
