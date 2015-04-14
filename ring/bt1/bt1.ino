#include <SoftwareSerial.h>

int bluetoothTx = 2;
int bluetoothRx = 3;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  // begin the erial monitor at 9600 bps
  bluetooth.begin(115200);  // the bluetooth mat defaults to 115200 bps
  bluetooth.print('$');
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
}

void loop() {
  // put your main code here, to run repeatedly:
  if(bluetooth.available()) {
    Serial.print((char)bluetooth.read()); 
  }
  if(Serial.available()) {
    bluetooth.print((char)Serial.read()); 
  }
}
