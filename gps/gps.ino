// #include <SoftwareSerial.h>
 
// SoftwareSerial mySerial(25, 26); // RX, TX
HardwareSerial mySerial(0); 
 
void setup() {
 // Open serial communications and wait for port to open:
 Serial.begin(57600);
 mySerial.begin(9600, SERIAL_8N1, 33, 34);
 while (!Serial) {
 ; // wait for serial port to connect. Needed for native USB port only
 }
 
 
 Serial.println("Goodnight moon!");
 
 // set the data rate for the SoftwareSerial port
 mySerial.begin(9600);
 mySerial.println("Hello, world?");
}
 
void loop() { // run over and over
 if (mySerial.available()) {
 Serial.write(mySerial.read());
 }
 if (Serial.available()) {
 mySerial.write(Serial.read());
 }
}
