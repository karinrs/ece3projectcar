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
const double kP = 0.07;
const double kD = 1.4; //subir tal vez
const double kI = 0.01;

//Speed Constants
const int baseSpeed = 180;
double pastError = 0;
double integralError = 0;

int lineCount = 0;
bool turned = false;
bool prevReadBlack = false;

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

void turnAround() {
  //brake
  // analogWrite(left_pwm_pin, 0);
  // analogWrite(right_pwm_pin, 0);
  // delay(400);

  //set motors to spin in opposite directions
  digitalWrite(left_dir_pin, HIGH);   //left motor backwards
  digitalWrite(right_dir_pin, LOW);    //right motor forwards
  
  analogWrite(left_pwm_pin, 255);
  analogWrite(right_pwm_pin, 255);
  delay(215);

  analogWrite(left_pwm_pin, 0);
  analogWrite(right_pwm_pin, 0);
  delay(100); //ver si 100 esta bien o subirlo de vuelta a 200
  //reset motor directions
  digitalWrite(left_dir_pin, LOW);
  digitalWrite(right_dir_pin, LOW);
  analogWrite(left_pwm_pin, 130);
  analogWrite(right_pwm_pin, 130);
  delay(200);
  //clear error to prevent overcorrection
  pastError = 0;
  integralError = 0;

  turned = true;
}

void stopCar() {
  analogWrite(left_pwm_pin, 0);
  analogWrite(right_pwm_pin, 0);
  digitalWrite(left_nslp_pin, LOW); //motors to sleep
  digitalWrite(right_nslp_pin, LOW);
  
  while(true) {
    delay(1000);
  }
}

void loop() {

  ECE3_read_IR(sensorValues);

  int blackSensorCount = 0;
  for (int i = 1; i < 7; i++) {
    if (sensorValues[i] > 2100) {
      blackSensorCount++;
    }
  }

  if (blackSensorCount >= 4) {
    lineCount++;
    prevReadBlack = true;

    if (lineCount >= 2 && !turned) {
      turnAround();
      lineCount = 0;
    } 
    else if (lineCount >= 2 && turned) {
      stopCar();
    }
  }

  else{
    if (prevReadBlack)
    {
      prevReadBlack = false;
      lineCount = 0;
    }
  }

  //weighted error
  double currentError = 0;


  for (int i = 0; i < 8; i++) {
    currentError += ((float)(sensorValues[i] - minimums[i]) * 1000.0 * weights[i]) / (maximums[i] - minimums[i]);
  }
  currentError = currentError / 8.0;

  integralError += currentError;
  integralError = constrain(integralError, -2000.0, 2000.0);


  //PD controller
  double derivative = currentError - pastError;
  double motorControl = (kP * currentError) + (kI * integralError) + (kD * derivative);


  //update motor speeds
  int leftSpd = baseSpeed - motorControl;
  int rightSpd = baseSpeed + motorControl;


  //motor output
  analogWrite(left_pwm_pin, constrain(leftSpd, 0, 255));
  analogWrite(right_pwm_pin, constrain(rightSpd, 0, 255));


  pastError = currentError;
}

