#include <Servo.h>

Servo servo;

const int potentiometerControlPin = A0;
int potentiometerValue;
int angle;

void setup() {
  servo.attach(9);

  Serial.begin(9600);
}

void loop() {
  potentiometerValue = analogRead(potentiometerControlPin);
  Serial.print("potentiometer value: ");
  Serial.print(potentiometerValue);

  angle = map(potentiometerValue, 0, 1023, 0, 179);
  Serial.print(", angle: ");
  Serial.println(angle);

  servo.write(angle);
  delay(15);
}
