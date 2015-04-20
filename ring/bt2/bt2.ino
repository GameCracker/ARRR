char INTBYTE;
int LED = 13;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Press 1 to turn Arduino pin 13 LED ON or 0 to turn it OFF:");
  while(!Serial.available());
  INBYTE = Serial.read();
  if(INBYTE == '0')
    digitalWrite(LED, LOW);
  if(INBYTE == '1')
    digitalWrite(LED, HIGH);
  delay(50);
}
