#include <ECE3.h>



uint16_t sensorValues[8]; // right -> left, 0 -> 7

const int minimums[] = {619, 596, 687, 596, 619, 711, 687, 711};
const int maximums[] = {1881, 1904, 1812, 1904, 1881, 1789, 1813, 1789};
const int weights[] = {-8, -4, -2, -1, 1, 2, 4, 8};

const int left_nslp_pin=31; // nslp ==> awake & ready for PWM
const int left_dir_pin=29;
const int left_pwm_pin=40;
const int right_nslp_pin=11;
const int right_dir_pin=30;
const int right_pwm_pin=39;

const int photoPin1 = 65; // left side of car
const int photoPin2 = 48;
const int photoPin3 = 64;
const int photoPin4 = 47;
const int photoPin5 = 52;
const int photoPin6 = 68;
const int photoPin7 = 53;
const int photoPin8 = 69; // right side of car

const int LED_RF = 41;

///////////////////////////////////
void setup() {
// put your setup code here, to run once:
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);

  pinMode(LED_RF, OUTPUT);
  
//  ECE3_Init();

// set the data rate in bits/second for serial data transmission
  Serial.begin(9600); 
  delay(2000); //Wait 2 seconds before starting 
  
}

void loop() {
  // put your main code here, to run repeatedly: 
  int leftSpd = 70;
  
  ECE3_read_IR(sensorValues);

  analogWrite(left_pwm_pin,leftSpd);

// 
  
//  ECE3_read_IR(sensorValues);

  digitalWrite(LED_RF, HIGH);
  delay(250);
  digitalWrite(LED_RF, LOW);
  delay(250);
    
  }
