char val;  // variable to receive data from serial port
int ledpin = 2;  // LED connected to pin 2

void setup() {
  // put your setup code here, to run once:
  pinMode(ledpin = 10, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    val = Serial.read();
    if(val == '0') {
      digitalWrite(ledpin, LOW);  // turn off pin 13 off
      delay(1000);
      Serial.println("10 off");
    }
    if(val == '1') {
      digitalWrite(ledpin = 10, HIGH);
      delay(1000);
      Serial.println("10 on");
    }
  }
}p
