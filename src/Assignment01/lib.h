#ifndef __LIBH__
#define __LIBH__

// Library to manage a timer
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

// Seconds to wait to go in sleep mode
#define SEC_TO_SLEEP 10000

// Tell if the button pressed is NOT correct
// (the corresponding led is NOT in the pattern)
#define INCORRECT 0

// Tell if the button pressed is correct
// (the corresponding led is in the pattern)
#define CORRECT 1

// Tell if a button has already been pressed
#define TAKEN 2

// Initialize the game
void initialize();

// Initialize the pins for input and output
void initializePins();

// Initialize the variables
void initializeVariables();

// Enable buttons interrupts
void setInterrupts();

// Set up the initial state
void initialState();

// Start the game and checks if it is over
void inGame();

// Set up the game at the given difficulty 
void startGame(int difficulty);

// Show the pattern to the player
void showPattern();

// Rapresents a turn of the game
void play();

// Arduino goes in deep sleep
void sleep();

// Arduino wakes up from sleeping
void wakeUp();

// Fading of the red led
void fading();

// Set all the game leds at the given state
void setLedsState(int state);

// Return true if the button given is pressed
bool isButtonPressed(int button);

// Increments the number of penalties, msg is the penalty message to display
void addPenalty(String msg);

// Create the random pattern for the leds, returns the number of leds on
int createPattern();

#endif
