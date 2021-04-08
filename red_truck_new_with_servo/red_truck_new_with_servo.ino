#include <Servo.h>;
Servo servo;
// length of serial input buffer
#define LENGTH 20

// analog pins
#define hull A8
#define steer A2
#define battery12 A3

// digital pins
#define throttle_servo 9
#define steer_relay1 3
#define steer_relay2 4
#define key 5
#define reverse_relay 2

// batteries
int current_battery_48 = 0;
int battery_threshold_48 = 50;
int current_battery_12 = 0;
int battery_threshold_12 = 50;

// steering
int min_pot = 380 ;
int max_pot = 713 ;
int degree = 0;

// stearing bias and logic gate
int min_max_bias = 10;
bool goesLeft = false;
bool goesRight = false;

// throttle
int current_speed = 0;
int min_servo = 1700;
int max_servo = 1000;
int stop_car = 1700;

// intial wip
int forward_speed_car = 1550;
int backward_speed_car = 1500;

// car state
bool _stop = true;
int current_servo = stop_car;

// rate of servo
int up_down_state = 1;

// hold wip
int reverse_slope = 1575;

// forward or reverse
bool reverse = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Getting ready");
  Serial.println("Setting pinmode");
  pinMode(hull, INPUT);
  pinMode(steer, INPUT);
  pinMode(battery12, INPUT);

  pinMode(reverse_relay, OUTPUT);
  pinMode(steer_relay1, OUTPUT);
  pinMode(steer_relay2, OUTPUT);
  pinMode(key, OUTPUT);
  Serial.println("Setting key to off");
  digitalWrite(key, HIGH);
  Serial.println("Writing to default servo");
  servo.attach(throttle_servo);
  servo.writeMicroseconds(current_servo);
  delay(1000);
  digitalWrite(reverse_relay, HIGH);
  digitalWrite(key, LOW);

  Serial.println("Starting in one second");
  delay(1000);
}

// function to delimit the string
int getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]).toInt() : -1;
}


void loop() {
  //  analog read
  int steer_pot = analogRead(steer);
  int hull_voltage = analogRead(hull);
  int battery_voltage = analogRead(battery12);
  Serial.print("hull "); Serial.print(hull_voltage);
  degree = map(steer_pot, min_pot + min_max_bias, max_pot - min_max_bias, 0 , 60);
  current_speed = map(hull_voltage, 0, 1023, 0, 50 );
  current_battery_12 = map(battery_voltage, 0, 1023, 0, 100);


  //  read from serial
  if (Serial.available()) {
    char buffer[LENGTH];
    int index = 0;
    bool receiving = true;

    while (receiving) {
      if (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\n' || ch == '\0') {
          buffer[index] = '\0';
          receiving = false;
        } else {
          buffer[index++] = ch;
          if (index == LENGTH) {
            buffer[index] = '\0';
            break;
          }
        }
      }
    }
    //    values from serial
    int _speed = getValue(buffer, '#', 0);
    int _steer = getValue(buffer, '#', 1);
    _speed = _speed != -1 ? _speed : 0;
    _steer = _steer != -1 ? _steer : 30;


    if (_speed < 0) {
      reverse = true;
    } else if (_speed > 0) {
      reverse = false;
    }

    //    absolute to serve same values to servo
    _speed = abs(_speed);

    if (_speed != 0) {
      _stop = false;
    } else {
      _stop = true;
      current_speed = 0;
    }

    //    run servo to reach the given speed from current speed
    if (current_speed < _speed) {
      if (current_servo > max_servo) {
        current_servo = current_servo - up_down_state;
      }
    } else if (current_speed > _speed) {
      if (current_servo < min_servo) {
        current_servo = current_servo + up_down_state;
      }
    }

    //    check if servo reaches min or max threshold
    if (current_servo < max_servo) {
      current_servo = max_servo;
    }
    if (current_servo > min_servo) {
      current_servo = min_servo;
    }

    //    reverse relay logic
    if (reverse == true) {
      digitalWrite(reverse_relay, LOW);
    } else {
      digitalWrite(reverse_relay, LOW);
    }

    //    stopping servo value
    if (_stop == true) {
      current_servo = 1700;
    }

    //  steering logic

    if (_steer >= 0 && _speed <= 60) {
      if (_steer > 30) {
        degree = map(steer_pot, min_pot + min_max_bias, max_pot - min_max_bias, 0 , 60);
        if (_steer != degree) {
          goesLeft = true;
          goesRight = false;
        }
      }
      else if (_steer < 30) {
        degree = map(steer_pot, min_pot + min_max_bias, max_pot - min_max_bias, 0 , 60);
        if (_steer != degree) {
          goesRight = true;
          goesLeft = false;
        }

      } else if (_steer == 30) {
        goesRight = false;
        goesLeft = false;
        digitalWrite(steer_relay2, LOW);
        digitalWrite(steer_relay1, LOW);
      }
    }

    //    write servo
    servo.writeMicroseconds(current_servo);

    //    steering gate
    if (goesRight == true &&  steer_pot >= min_pot + min_max_bias) {
      digitalWrite(steer_relay1, LOW);
      digitalWrite(steer_relay2, HIGH);
    } else if (goesLeft == true && steer_pot <= max_pot - min_max_bias) {
      digitalWrite(steer_relay2, LOW);
      digitalWrite(steer_relay1, HIGH);
    } else {
      goesRight = false;
      goesLeft = false;
      digitalWrite(steer_relay2, LOW);
      digitalWrite(steer_relay1, LOW);
    }

    /*    debug
        0: current speed; 1: current degree; 2: current 12 volt battery level, 3: reverse state
        4: given speed, 5: given steering angle, 6: given serial input
    **/
    Serial.print(current_speed); Serial.print(",");
    Serial.print(degree); Serial.print(",");
    Serial.print(current_battery_12); Serial.print(",");
    Serial.print(reverse); Serial.print(",");
    Serial.print(_speed); Serial.print(",");
    Serial.print(_steer); Serial.print(",");
    Serial.print(hull_voltage); Serial.print(",");
    Serial.print(current_servo); Serial.print(",");
    Serial.println(buffer);
    //
  }
}
