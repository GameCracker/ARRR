/*
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
}*/


#define trigPin 13
#define echoPin 12
#define led 11
#define led2 10

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  Serial.println("Hello world");
}

void loop() {
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  if (distance < 4) {  // This is where the LED On/Off happens
    digitalWrite(led,HIGH); // When the Red condition is met, the Green LED should turn off
  digitalWrite(led2,LOW);
  Serial.println("red on");
}
  else {
    digitalWrite(led,LOW);
    digitalWrite(led2,HIGH);
    Serial.println("green on");
  }
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
  else {
    Serial.print(distance);
    Serial.println(" cm");
  }
  //delay(500);
}
