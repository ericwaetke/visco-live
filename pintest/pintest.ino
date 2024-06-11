void setup() {
  // put your setup code here, to run once:
  pinMode(4, INPUT);
  pinMode(5, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print("Pin 4:");
    Serial.print(digitalRead(4));
    Serial.print(",");
    Serial.print("Pin 5:");
    Serial.println(digitalRead(5));
}
