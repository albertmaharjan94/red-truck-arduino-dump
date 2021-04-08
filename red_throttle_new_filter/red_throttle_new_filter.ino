#include "MegunoLink.h"
#include "Filter.h"
 
#include <Servo.h>;
#define reverse_relay 2
#define hull A7
#define steer A2

int _speed = 11;
int current_speed = 0;

int min_servo = 1700;
int max_servo = 1000;

Servo servo;
int stop_car = 1700;
int forward_speed_car = 1550;
int backward_speed_car = 1500;
bool _stop = true;
int current_servo = stop_car;

int up_down_state = 1;

int reverse_slope=1575;


// Create a new exponential filter with a weight of 5 and an initial value of 0. 
long FilterWeight = 2;
ExponentialFilter<long> ADCFilter(FilterWeight, 0);

void setup() {
  // put your setup code here, to run once:
  servo.attach(9);
  servo.writeMicroseconds(current_servo);
  pinMode(reverse_relay, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(hull, INPUT);
  pinMode(steer, INPUT);
  digitalWrite(reverse_relay, HIGH);
  delay(1000);
  digitalWrite(5, LOW);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int hull_voltage = analogRead(hull);
  
  ADCFilter.Filter(hull_voltage);
  int cf = ADCFilter.Current();
  
  int current_speed = map(cf, 0,1023,0, 50 );
  int steer_voltage = analogRead(steer);
  if(Serial.available()){
    char data = Serial.read();
    if(data == 'w'){
      digitalWrite(reverse_relay, HIGH);
      current_servo = 1600;
      _stop = false;
      
    }else if(data=='s'){
      current_servo = stop_car; 
      _stop = true;
    }else if(data == 'x'){
      digitalWrite(reverse_relay, LOW); 
      current_servo = 1550;
      _stop = false;
    }
  }
  if(current_speed < _speed){
      if(current_servo > max_servo){
        Serial.print(" before minus : ");Serial.print(current_servo);
        current_servo = current_servo-up_down_state;
        Serial.print(" minus : ");Serial.print(current_servo);
      } 
    }else if(current_speed > _speed){
     if(current_servo < min_servo){
        Serial.print(" before plus : ");Serial.print(current_servo);
        current_servo = current_servo+up_down_state;
        Serial.print(" plus : ");Serial.print(current_servo);
    } 
  }
  if(current_servo< max_servo){
    current_servo = max_servo;
  }
  if(current_servo > min_servo){
    current_servo = min_servo;
  }
  if(_stop == true){
    current_servo = 1700;    
  }
  servo.writeMicroseconds(current_servo);
  
  Serial.print(" HULL: ");Serial.print(hull_voltage);
  Serial.print(" SPEED: ");Serial.print(current_speed);
  Serial.print(" Current Servo: ");Serial.println(current_speed);
  
  delay(35);
//  Serial.print(" STEER: ");Serial.println(steer_voltage);
  
}
