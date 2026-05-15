#include <ECE3.h>

uint16_t sensorValues[8];


//Calibration values
const int minimums[] = {619, 596, 687, 596, 619, 711, 687, 711};
const int maximums[] = {1881, 1904, 1812, 1904, 1881, 1789, 1813, 1789};
const float weights[] = {-8.0, -4.0, -2.0, -1.0, 1.0, 2.0, 4.0, 8.0};


//Motor Pins
const int left_nslp_pin = 31;
const int left_dir_pin = 29;
const int left_pwm_pin = 40;
const int right_nslp_pin = 11;
const int right_dir_pin = 30;
const int right_pwm_pin = 39;


//PD Constants
const double kP = 0.08;
const double kD = 0.25;


//Speed Constants
const int baseSpeed = 150;
double pastError = 0;


void setup() {
  ECE3_Init();


  // Motor Setup
  pinMode(left_nslp_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(left_pwm_pin, OUTPUT);
  pinMode(right_nslp_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);
  pinMode(right_pwm_pin, OUTPUT);


  digitalWrite(left_nslp_pin, HIGH); //awake
  digitalWrite(right_nslp_pin, HIGH); //awake
  digitalWrite(left_dir_pin, LOW);
  digitalWrite(right_dir_pin, LOW);


  Serial.begin(9600);
  delay(2000);
}


void loop() {
  ECE3_read_IR(sensorValues);


  //weighted error
  double currentError = 0;


  for (int i = 0; i < 8; i++) {
    currentError += ((float)(sensorValues[i] - minimums[i]) * 1000.0 * weights[i]) / (maximums[i] - minimums[i]);
  }
  currentError = currentError / 8.0;


  //PD controller
  double derivative = currentError - pastError;
  double motorControl = (kP * currentError) + (kD * derivative);


  //update motor speeds
  int leftSpd = baseSpeed - motorControl;
  int rightSpd = baseSpeed + motorControl;


  //turning
  if (motorControl < -40) { //turning left
      leftSpd = leftSpd * 0.7;
  }
  else if (motorControl > 40) {//turning right
      rightSpd = rightSpd * 0.7;
  }


  //motor output
  analogWrite(left_pwm_pin, constrain(leftSpd, 0, 255));
  analogWrite(right_pwm_pin, constrain(rightSpd, 0, 255));


  pastError = currentError;
}
