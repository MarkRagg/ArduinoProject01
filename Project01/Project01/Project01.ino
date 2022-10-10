#define LED_PIN_ROSSO 9
#define LED_PIN1 10
#define LED_PIN2 11
#define LED_PIN3 12
#define LED_PIN4 13
#define BUTTON_PIN1 2
#define BUTTON_PIN2 3
#define BUTTON_PIN3 4
#define BUTTON_PIN4 5

#include <avr/sleep.h>

int pressed;
int brightness;
int fadeAmount;
int currIntensity;

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
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN1), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN2), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN3), wakeUp, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN4), wakeUp, RISING);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN2);
  Serial.println("\nWelcome to the Catch the Led Pattern Game. Press Key T1 to Start\n");
  analogWrite(LED_PIN_ROSSO, currIntensity);   
  currIntensity = currIntensity + fadeAmount;
  if (currIntensity == 0 || currIntensity == 255) {
    fadeAmount = -fadeAmount ; 
  }     
  delay(150);               

  if (buttonState == HIGH) {
    digitalWrite(LED_PIN1, HIGH);
    Serial.println("ON");
  } else {
    digitalWrite(LED_PIN1, LOW);
    Serial.println("OFF");
 }
}

void initialize(){
  for(int i = 9; i<14; i++) {
    pinMode(i, OUTPUT);
  }
  for(int i = 2; i<6; i++) {
    pinMode(i, INPUT);
  }
}

void sleep(){
  //Serial.println("GOING IN POWER DOWN IN 10s ...");
  //Serial.flush();
  //delay(10000);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}
