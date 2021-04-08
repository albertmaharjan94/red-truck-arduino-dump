#define power_pin 9
#define reverse_relay 2
#define steer_relay1 3
#define steer_relay2 4

// steering analog
#define steer A2


int voltage_bias = 0;
int stop_voltage = 0.83 * 51;

// initial forward to max threshold
int min_forward = 1.68 * 51;
int max_forward = 1.85 * 51;

// initial backward to max threshold
int min_backward = 1.90 * 51;
int max_backward = 2 * 51;

// initial voltage speed
int voltage = stop_voltage;

String state = "STOP";

int min_pot = 385 ;
int max_pot = 690 ;
int degree = 0;


int min_max_bias = 10;
bool goesLeft = false;
bool goesRight = false;


int delay_threshold = 3.5;
// function init
int getValue(String data, char separator, int index);


void setup() {
  //  power pin
  pinMode(power_pin, OUTPUT);
  // reverse for power
  pinMode(reverse_relay, OUTPUT);

  //  forward/reverse motor for steering
  pinMode(steer_relay1, OUTPUT);
  pinMode(steer_relay2, OUTPUT);

  //  analog pot
  pinMode(steer, INPUT);

  // Init to forward relay
  digitalWrite(reverse_relay, HIGH);
  Serial.begin(115200);

  //  Calibrate to middle degree 30
  int _tmp_steer = analogRead(steer);
  int _tmp_degree = map(_tmp_steer, min_pot, max_pot, 0 , 60);
  Serial.println("Calibrating to mid degree");
  //  if (_tmp_degree < 30){
  //    while(_tmp_degree == 30){
  //       _tmp_degree += 1;
  //       digitalWrite(steer_relay2, LOW);
  //       digitalWrite(steer_relay1, HIGH);
  //    }
  //  }else if(_tmp_degree > 30){
  //    while(_tmp_degree == 30){
  //       _tmp_degree -= 1;
  //       digitalWrite(steer_relay1, LOW);
  //       digitalWrite(steer_relay2, HIGH);
  //    }
  //  }
  degree = _tmp_degree;
  Serial.println("Back to mid degree");

}
void stop_car();
void move_forward();
void move_backward();

void loop() {
  int steer_pot = analogRead(steer);
  degree = map(steer_pot, min_pot + min_max_bias, max_pot - min_max_bias, 0 , 60);

  //  Serial.flush();
  if (Serial.available()) {
    char data = Serial.read();

    if (data == 's') {
      stop_car();
    }
    if (data == 'w') {
      move_forward();
    }
    if (data == 'x') {
      move_backward();
    }

  }



  analogWrite(power_pin, voltage);
  delay(delay_threshold);
}

void stop_car() {
    
  voltage = stop_voltage;
    // decrease speed linearly
//    for (int i = voltage; i >= stop_voltage; i = i - 2) {
//      voltage = i;
//      analogWrite(power_pin, voltage);
//      Serial.print(state); Serial.print("  ");
//      Serial.println(voltage);
//      delay(delay_threshold);
//    }
  state = "STOP";
}

void move_forward() {
  if (state == "BACKWARD") {
    stop_car();
  }

  // switch relay for forward
  digitalWrite(reverse_relay, HIGH);
  if (state == "BACKWARD" || state == "STOP" ) {
    voltage = max_forward;

//    linear 
    //    for (int i = min_forward; i <= max_forward; i = i + 2) {
    //      voltage = i;
    //      analogWrite(power_pin, voltage);
    //      Serial.print(state); Serial.print("  ");
    //      Serial.println(voltage);
    //      delay(delay_threshold);
    //    }
  }
  digitalWrite(reverse_relay, HIGH);
  state = "FORWARD";
}

void move_backward() {
  if (state == "FORWARD") {
    stop_car();
  }
  // switch relay for backward
  digitalWrite(reverse_relay, LOW);
  if (state == "FORWARD" || state == "STOP") {
    // increase speed linearly
    voltage = max_backward;
//    for (int i = min_backward; i <= max_backward; i = i + 2) {
//      voltage = i;
//      analogWrite(power_pin, voltage);
//      Serial.print(state); Serial.print("  ");
//      Serial.println(voltage);
//      delay(delay_threshold);
//    }

  }
  state = "BACKWARD";

}
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
