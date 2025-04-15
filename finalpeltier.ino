//This version was last updated 4/3 5:48 pm
   
//jeffreys initialization
const int AIN1 = 13;           //control pin 1 on the motor driver for the right motor
const int AIN2 = 12;            //control pin 2 on the motor driver for the right motor
const int PWMA = 11;            //speed control pin on the motor driver for the right motor
int switchPin = 7;             //switch to turn the robot on and off --> leftover code, may delete, not referenced
int sensorPin = 1;   //temperature read pin
int motorValueMax = 255;       //starting value for motorcontroller -> used for testing voltage control via motor controller -Jeffrey
int motorValue = 255;
int timecount = 0;
#define ADC_VREF_mV    5000.0 // in millivolt
#define ADC_RESOLUTION 1024.0
#define PIN_LM35       A1
int ntc = A0; // Declaration of the sensor input pin
// Declaration of temporary variables
double raw_value;
double voltage;
double temperature;

int HEATMODEINT = 7; //MODECONTROL: THIS IS WHAT CONTROLS WHAT MODE THE PELTIER STRIP IS ON
//Shivank Initialization
#include <Stepper.h>
const int stepsPerRevolution = 2038;           // Full rotation for 28BYJ-48
const int totalRotations = 4;                  // Total movement range: 4 full turns
const int maxSteps = stepsPerRevolution * totalRotations;  // 8152 steps = 100%

// Stepper wiring: IN1, IN3, IN2, IN4
Stepper myStepper(stepsPerRevolution, 2, 4, 3, 5);

// Current absolute step position from 0% base
int currentStepPosition = 0;

void setup() {
  //jeffreys chunk:
  //motor controller setup
  pinMode(switchPin, INPUT_PULLUP); //leftover code for a switch, not referenced and I might delete -Jeffrey
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  //Begin serial monitor
  //a0 temp initialize
  pinMode(ntc, INPUT);
  Serial.begin(9600);               
  //leftover code -- do not delete this -jeffrey -> //Serial.println("Enter motor speed (0-255 (or -255 to 255))... ");  //Prompt to get input in the serial monitor.

  //Shivank Setup
  
  myStepper.setSpeed(15);  // Reasonable speed for 28BYJ-48
  Serial.println(maxSteps);
  Serial.println("Stepper ready.");
  Serial.println("Enter target percent (0 - 100):");
}

void loop() {
  //jeffreys chunk:
  //LEFTOVER CODE COMMENTS BUT DO NOT DELETE THIS CHUNK
  //serial monitor input
 /* if (Serial.available() > 0) { 
    motorSpeed = Serial.parseInt();     //set the motor speed equal to the number in the serial message

    Serial.print("Motor Speed: ");      //print the speed that the motor is set to run at
    Serial.println(motorSpeed);
  }*/
  //spinMotor(255); //passes a value from -255 to 255 (corresponds -9V to +9V) to the motor controller, which controls the peltier cell

  //MODECONTROL:
  int direction = 1;
  int starttimeSeconds = 0;
  int stoptimeSeconds = 5;
  switch (HEATMODEINT) {
    case 1: //OFF 
      starttimeSeconds = 0;
      stoptimeSeconds = 5;
      direction = 1;
      break;
    case 2: //MILD HEAT
      starttimeSeconds = 1;
      stoptimeSeconds = 3;
      direction = 1;
      break;
    case 3: //MEDIUM HEAT
      starttimeSeconds = 2;
      stoptimeSeconds = 3;
      direction = 1;
      break;
    case 4: //HIGH HEAT
      starttimeSeconds = 5;
      stoptimeSeconds = 5;
      direction = 1;
      break;
    case 5:  //MAX HEAT
      starttimeSeconds = 10;
      stoptimeSeconds = 2;
      direction = 1;
      break;
    case 6: //COOL 1
      starttimeSeconds = 28;
      stoptimeSeconds = 24; //changed
      direction = -1;
      break;
    case 7: //COOL 2
      starttimeSeconds = 10;
      stoptimeSeconds = 16;
      direction = -1; //partial voltage
      break;
    case 8: //MAXTESTING DELETE LATER
      starttimeSeconds = 00;
      stoptimeSeconds = 0;
      direction = 1;
      break;
  }
  //MODECONTROL: Do not touch, actual implementation of peltier control
  int starttime = 2 * starttimeSeconds;                                                                                                                                                                                                                                                                                                                              
  int stoptime = starttime + 2 * stoptimeSeconds;
  if(timecount<=starttime){ //x * 500ms -> # of seconds of voltage
    spinMotor(255 * direction); //motorValue
    Serial.print(timecount);
    Serial.println("START");
  }
  else if(timecount<(stoptime)){ //(y - x) * 500ms -> # of seconds of cooling
    spinMotor(0);
    Serial.println("STOP");
  }
  if(timecount>=stoptime){
    timecount = 0;
    Serial.println("STOP");
  }
  timecount = timecount + 1;
  
  /*
  //note: i am using a motor controller that feeds a dc motor voltage in increments between 0 to +-255 to control the peltier cells by swapping the DC motor for a peltier strip
  //
  // get the ADC value from the temperature sensor
  int adcVal = analogRead(PIN_LM35);
  // convert the ADC value to voltage in millivolt
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
  // convert the voltage to the temperature in Celsius
  float tempC = milliVolt / 10;
  // convert the Celsius to Fahrenheit
  float tempF = tempC * 9 / 5 + 32;

  // print the temperature in the Serial Monitor:
  Serial.print("Temperature: ");
  Serial.print(tempC);   // print the temperature in Celsius
  Serial.print("°C");
  Serial.print("  ~  "); // separator between Celsius and Fahrenheit
  Serial.print(tempF);   // print the temperature in Fahrenheit
  Serial.println("°F");
  //keep delay here, refreshes at a rate of 1 loop per 1/2 second. subject to change probably
  */
  raw_value = analogRead(ntc); 
  // Read out the voltage using an analog value
  voltage = raw_value * 5.0 / 1023.0;
  // Calculation of the temperature using the voltage
  temperature = ((voltage / 5.0) * 10000.0) / (1.0 - (voltage / 5.0));
  temperature = 1.0 / ((1.0 / 298.15) + (1.0 / 3950.0) * log(temperature / 10000.0));
  temperature = temperature - 273.15;
  temperature = temperature * 9 / 5 + 32;
  // Output of the measured value
  //Serial.println("Temperature: " + String(temperature) + " °F");

  int temperature_reading = 0;
  int safe_temp = 150;
  if(temperature_reading>=safe_temp){
    HEATMODEINT = 1; //killswitch condition, not fully implemented
  }

  delay(500); //do not change from 500

  //Shivank's Chunk
  if (Serial.available() > 0) {
    int targetPercent = Serial.parseInt();//data inputed from app

    // Validate percent input
    if (targetPercent >= 0 && targetPercent <= 100) {
      Serial.println(targetPercent);
      Serial.println(maxSteps);
      int targetSteps = (int)(((double)targetPercent / 100.0) * maxSteps);

      Serial.println(targetSteps);
      int stepsToMove = targetSteps - currentStepPosition;
      Serial.println(stepsToMove);

      // Move stepper to the absolute position
      myStepper.step(stepsToMove);
      currentStepPosition = targetSteps;

      Serial.print("Moved to ");
      Serial.print(targetPercent);
      Serial.println("% of 4 rotations.");
    } else {
      Serial.println("Invalid input. Please enter a number between 0 and 100.");
    }

    // Clear buffer and prompt again
    while (Serial.available() > 0) Serial.read();
    Serial.println("Enter target percent (0 - 100):");
    Serial.println(currentStepPosition);
  }

}

//jeffreys chunk:
//motorcontroller function, code for actually operating it to control the peltier cells
//ignore this function and dont edit
//skibidi
void spinMotor(int motorValue)
{
  if (motorValue > 0)
  {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  }
  else if (motorValue < 0)
  {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  }
  else
  {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }
  analogWrite(PWMA, abs(motorValue));
}
