int pirState = LOW;

void setupPIR()
{
  pinMode(PIR_PIN, INPUT);
}

void loopPIR()
{
  // change detection
  int input = digitalRead(PIR_PIN);
  if (input == HIGH && pirState == LOW) {
    pirState = HIGH;
    Serial.println("PIR: Motion detected!");
  }
  else if (input == LOW && pirState == HIGH)
  {
    pirState = LOW;
    Serial.println("PIR: No motion!");
  }
}
