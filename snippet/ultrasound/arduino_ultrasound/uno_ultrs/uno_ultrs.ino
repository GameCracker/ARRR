#define trigPin 13
#define echoPin 12
#define led 11
#define led1 10

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led1, OUTPUT); 
}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/29.1;
  if(distance < 4) {
    digitalWrite(led, HIGH);
    digitalWrite(led1, LOW);
  }
  else {
    digitalWrite(led, LOW);
    digitalWrite(led1, HIGH);  
  }
  if(distance >= 200 || distance <= 0) {
    Serial.println("Out of range"); 
  }
  else {
    Serial.print(distance);
    Serial.println(" cm"); 
  }
  delay(500);
}
