/*
Authors:
Dilaver Shtini,     dilaver.shtini@studio.unibo.it,       0000974568
Francesco Carlucci, francesco.carlucci6@studio.unibo.it, 0000977003
Marco Raggini,      marco.raggini2@studio.unibo.it,      0000
*/

#include "lib.h"

extern int state;

void setup() {
  initialize();

  enableInterrupt(BUTTON_PIN1, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN2, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN3, wakeUp, RISING);
  enableInterrupt(BUTTON_PIN4, wakeUp, RISING);
}

void loop() {
  delay(5);  //delay per fading

  switch (state) {
    case INITIAL_STATE:
      initialState();
      break;
      
    case IN_GAME:
      inGame();
      break;
  }
}


