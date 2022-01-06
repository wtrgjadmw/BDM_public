int val = 0;
int led = 13;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9800);
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(led, HIGH);

  delay(1000);

  val = analogRead(0);
  Serial.println(val/4);

  delay(2000);

  digitalWrite(led, LOW);

  delay(2000);

}
