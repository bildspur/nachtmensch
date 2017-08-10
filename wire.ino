boolean isWireOn = false;

void setupWire()
{
  wireOFF();
}

void wireOFF()
{
  isWireOn = false;
  analogWrite(EL_WIRE_PIN, 0);
}

void wireON()
{
  isWireOn = true;
  analogWrite(EL_WIRE_PIN, 255);
}

