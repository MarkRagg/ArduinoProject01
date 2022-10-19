#ifndef __LIBH__
#define __LIBH__

#include "Timer.h"

// Number of game leds
#define GAME_LEDS 4

// Number of buttons
#define GAME_BUTTONS 4

// Set pins for leds
#define LED_PIN_ROSSO 9
#define LED_PIN1 10
#define LED_PIN2 11
#define LED_PIN3 12
#define LED_PIN4 13

// Set pins for buttons
#define BUTTON_PIN1 2
#define BUTTON_PIN2 3
#define BUTTON_PIN3 4
#define BUTTON_PIN4 5

// Set pin for potentiometer
#define POTENTIOMETER A0

// States of the game
#define INITIAL_STATE 100
#define IN_GAME 101

// Button to start the game
#define START_GAME_BUTTON 0

// Max number of penalties
#define MAX_PENALTIES 3

// 
#define INCORRECT 0
#define CORRECT 1
#define TAKEN 2

void initialize();

void initializePins();

void initializeVariables();

void setInterrupts();

void initialState();

void inGame();

void sleep();

void wakeUp();

void fading();

void startGame(int difficulty);

void showPattern();

void play();

void setLedsState(int state);

bool isButtonPressed(int button);

void addPenalty(String msg);

int createPattern();

#endif
