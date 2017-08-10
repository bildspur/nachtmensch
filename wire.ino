boolean isWireOn = false;

void setupWire()
{
  wireOff();
}

void wireOff()
{
  isWireOn = false;
  analogWrite(EL_WIRE_PIN, 0);
}

void wireOn()
{
  isWireOn = true;
  analogWrite(EL_WIRE_PIN, 255);
}

