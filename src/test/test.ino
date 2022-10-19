/*
Authors:
  Dilaver Shtini,     dilaver.shtini@studio.unibo.it,      0000974568
  Francesco Carlucci, francesco.carlucci6@studio.unibo.it, 0000977003
  Marco Raggini,      marco.raggini2@studio.unibo.it,      0000970462
*/

#include "lib.h"

// Variable that describe the current state of the game
extern int state;

void setup() {
  initialize();
}

void loop() {
  delay(5);  // Delay for fading

  // Switch between the possible states of the game
  switch (state) {
    case INITIAL_STATE:
      initialState();
      break;
      
    case IN_GAME:
      inGame();
      break;
  }
}
