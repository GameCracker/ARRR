/*

*/
char val;
int ledpin = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledpin = 10, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    val = Serial.read();
    Serial.println("data read");
   // Serial.println(val);
    if(val == '0') {
      digitalWrite(ledpin, LOW);
      delay(1000);
      Serial.println("10 off");
    }  
    if(val == '1') {
      digitalWrite(ledpin = 13, HIGH);
      delay(1000);
      Serial.println("10 on");
    } 
  }
}
