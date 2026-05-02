int phototransistorValue;
int phototransistorMin = 1023;
int phototransistorMax = 0;

const int calibrationLedIndicatorPin = 13; 

void setup() {

  pinMode(calibrationLedIndicatorPin, OUTPUT);
  digitalWrite(calibrationLedIndicatorPin, HIGH);

  // Calibrate the phototransistor

  while(millis() < 5000)
  {
    phototransistorValue = analogRead(A0);
    if (phototransistorValue > phototransistorMax)
    {
      phototransistorMax = phototransistorValue;
    }
    if (phototransistorValue < phototransistorMin)
    {
      phototransistorMin = phototransistorValue;
    }
  }

  digitalWrite(calibrationLedIndicatorPin, LOW);
}

void loop() {
  phototransistorValue = analogRead(A0);
  int pitch = map(phototransistorValue, phototransistorMin, phototransistorMax, 50, 4000);
  tone(9, pitch, 20);
  delay(10);

}
