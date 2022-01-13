#include <AccelStepper.h>
#define dirPin 14    // D5
#define stepPin 12   // D6
#define motorInterfaceType 1
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
void setup()
{
    stepper.setMaxSpeed(100000);
}

void loop()
{
    stepper.setSpeed(1000);
    stepper.runSpeed();
}
