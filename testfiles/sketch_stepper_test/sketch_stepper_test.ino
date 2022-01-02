#include <AccelStepper.h>
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup()
{
    stepper.setMaxSpeed(100000);
    stepper.setSpeed(10000);
    stepper.setAcceleration(20000);
    stepper.moveTo(5000);
}

void loop()
{
    if (stepper.currentPosition() == 1000)
    {
        stepper.moveTo(2000);
    }
    if (stepper.currentPosition() == 2000)
    {
        stepper.moveTo(1000);
    }
    stepper.setSpeed(1000);
    stepper.runSpeedToPosition();
}