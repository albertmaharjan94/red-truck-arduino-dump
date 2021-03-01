#define power_pin 6
#define reverse_relay 2
#define led_relay 5

#define ultra_trig_1 9
#define ultra_echo_1 10
#define ultra_trig_2 11
#define ultra_echo_2 12


int voltage_bias = 0;

int stop_voltage = 0.83 * 51;

// initial forward to max threshold
int min_forward = 1.31 * 51;
int max_forward = 1.53 * 51;

// initial backward to max threshold
int min_backward = 1.60 * 51;
int max_backward = 2.15 * 51;

int voltage = 43;

float delay_threshold = 12.5;

bool drive = false;
bool flag = false;
// initial stop stage
String state = "STOP";

int max_ultrasonic = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(power_pin, OUTPUT);

  pinMode(reverse_relay, OUTPUT);

  //  ultrasonic
  pinMode(ultra_trig_1, OUTPUT);
  pinMode(ultra_echo_1, INPUT);
  pinMode(ultra_trig_2, OUTPUT);
  pinMode(ultra_echo_2, INPUT);

  digitalWrite(reverse_relay, HIGH);
  digitalWrite(led_relay, HIGH);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(ultra_trig_1, LOW);
  delayMicroseconds(2);
  digitalWrite(ultra_trig_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultra_trig_1, LOW);

  
  int duration = pulseIn(ultra_echo_1, HIGH);
  int distance = (duration * .0343) / 2;
    
  digitalWrite(ultra_trig_2, LOW);
  delayMicroseconds(2);
  digitalWrite(ultra_trig_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultra_trig_2, LOW);

  int duration2 = pulseIn(ultra_echo_2, HIGH);
  int distance2 = (duration2 * .0343) / 2;
  distance = 100;
  distance2= 100;
  if ((distance < max_ultrasonic && distance > 0) || (distance2 < max_ultrasonic && distance2 > 0)) {
    drive = false;
  } else {
    drive = true;
  }

  if (drive == false) {
    voltage = stop_voltage;
    flag = false;
    Serial.print("Obstacle detected  ");
  }
  else {
    if (state == "FORWARD" && flag == false) {
      for (int i = min_forward; i <= max_forward; i++) {
        voltage = i;
        analogWrite(power_pin, voltage);
        Serial.print(state); Serial.print("  ");
        Serial.println(voltage);
        delay(delay_threshold);
      }
    }

    else if (state == "BACKWARD" &&  flag == false) {
      for (int i = min_backward; i <= max_backward; i++) {
        voltage = i;
        analogWrite(power_pin, voltage);
        Serial.print(state); Serial.print("  ");
        Serial.println(voltage);
        delay(delay_threshold);
      }
    }
    flag = true;
  }
  
  if (Serial.available()) {
    char data = Serial.read();

    if (data == 'w') {
      if (drive == true) {
        if (state == "BACKWARD") {
          // decrease speed linearly
          for (int i = voltage; i >= stop_voltage; i--) {
            voltage = i;
            analogWrite(power_pin, voltage);
            Serial.print(state); Serial.print("  ");
            Serial.println(voltage);
            delay(delay_threshold);
          }
        }
        // switch relay for forward
        digitalWrite(reverse_relay, HIGH);
        if (state == "FORWARD") {
          // increase speed linearly
          for (int i = min_forward; i <= max_forward; i++) {
            voltage = i;
            analogWrite(power_pin, voltage);
            Serial.print(state); Serial.print("  ");
            Serial.println(voltage);
            delay(delay_threshold);
          }
        }
        if (state == "BACKWARD" || state == "STOP" ) {
          // increase speed linearly
          for (int i = min_forward; i <= max_forward; i++) {
            voltage = i;
            analogWrite(power_pin, voltage);
            Serial.print(state); Serial.print("  ");
            Serial.println(voltage);
            delay(delay_threshold);
          }
        }
      }
      digitalWrite(reverse_relay, HIGH);
      state = "FORWARD";
    }
    if (data == 's') {
      if (state == "FORWARD" || state == "BACKWARD") {
        // decrease speed linearly
        for (int i = voltage; i >= stop_voltage; i--) {
          voltage = i;
          analogWrite(power_pin, voltage);
          Serial.print(state); Serial.print("  ");
          Serial.println(voltage);
          delay(delay_threshold);
        }
      }
      state = "STOP";
    }
    if (data == 'x') {
      if (state == "FORWARD") {
        // decrease speed linearly
        for (int i = voltage; i >= stop_voltage; i--) {
          voltage = i;
          analogWrite(power_pin, voltage);
          Serial.print(state); Serial.print("  ");
          Serial.println(voltage);
          delay(delay_threshold);
        }
      }
      // switch relay for backward
      digitalWrite(reverse_relay, LOW);
      if (state == "FORWARD" || state == "STOP") {
        // increase speed linearly
        for (int i = min_backward; i <= max_backward; i++) {
          voltage = i;
          analogWrite(power_pin, voltage);
          Serial.print(state); Serial.print("  ");
          Serial.println(voltage);
          delay(delay_threshold);
        }
      }
      state = "BACKWARD";
    }
  }

  // standard debug
  Serial.print("Distance: "); Serial.print(distance); Serial.print("  ");
  Serial.print("Distance2: "); Serial.print(distance2); Serial.print("  ");
  Serial.print("State: "); Serial.print(state); Serial.print("  ");
  Serial.print("Voltage: "); Serial.println(voltage);
  analogWrite(power_pin, voltage);
}
