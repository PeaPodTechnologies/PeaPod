#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 516
#define RPM 10

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, 4, 5, 6, 7);

void setup() {
  Serial.begin(9600);
  stepper.setSpeed(RPM);
}

void loop() {
  stepper.step(STEPS);
  delay(1000);
}