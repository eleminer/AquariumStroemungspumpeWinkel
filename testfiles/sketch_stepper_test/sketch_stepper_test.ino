#include <AccelStepper.h>
#define dirPin 14    // D5
#define stepPin 12   // D6
#define enablepin 13 // D7
#define motorInterfaceType 1
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
void setup()
{
    stepper.setMaxSpeed(100000);
    stepper.setSpeed(10000);
    stepper.setAcceleration(20000);
    stepper.moveTo(-500000000);
}

void loop()
{
    stepper.setSpeed(1000);
    stepper.runSpeedToPosition();
}
