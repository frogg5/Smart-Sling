
//Shivank's initialization
#include <Stepper.h>
const int stepsPerRevolution = 2038; //steps per full rotation
Stepper myStepper(stepsPerRevolution, 1, 2, 3, 4); // Create stepper instance (pins: IN1, IN3, IN2, IN4)
long currentPosition = 0; // Tracks current position in steps from base

int max = 10;
int steps = 0;
int percent;  // target percent (set from app)
int percentC = 0;  // current percent (starts at 0)
int direction;


void setup() {
  //Shivank's part
  myStepper.setSpeed(15);  // RPM speed of the motor
}

void loop() {
  //shivank's part
  if (steps == 0 && percent != percentC) {
  // Calculate how many steps to move from current to new percentage
    int stepChange = (stepsPerRevolution * (percent - percentC) * max) / 100;

    steps = abs(stepChange);  // total steps to move
    direction = (stepChange > 0) ? 1 : -1;  // determine direction
  }

  // Move the motor one step at a time in the right direction
  if (steps > 0) {
    myStepper.step(direction);
    steps--;
    if (steps == 0) {
      percentC = percent; // update current percent after movement completes
    }
  }
}
