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
    motionPIR();
  }
  else if (input == LOW && pirState == HIGH)
  {
    pirState = LOW;
    noMotionPIR();
  }
}

void motionPIR()
{
  Serial.println("PIR: Motion detected!");
  wireON();
}

void noMotionPIR()
{
  Serial.println("PIR: No motion!");
  wireOFF();
}

