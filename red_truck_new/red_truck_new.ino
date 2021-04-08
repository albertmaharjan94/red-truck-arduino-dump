#define speed_pin A4
#define pot_pin A2
#define throttle_pin 9

int min_pot = 400 ;
int max_pot = 690 ;
int degree = 0;

float max_voltage = 80.0;

int min_max_bias = 10;

int speed_read = 0;
int pot_read = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(speed_pin, INPUT);
  pinMode(pot_pin, INPUT);
  pinMode(throttle_pin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()>0){
    char data = Serial.read();
    if(data == 'w'){
      analogWrite(throttle_pin, max_voltage);
    }else if(data == 's'){
      analogWrite(throttle_pin, 0);
    }
  }
  
  speed_read = analogRead(speed_pin);
  pot_read = analogRead(pot_read);

  Serial.print("Speed: "); Serial.print(speed_read);
  Serial.print("Pot: "); Serial.println(pot_read);
  delay(12.5);
}
