#include <AccelStepper.h>
#define dirPin 14    // D5
#define stepPin 12   // D6
#define enablepin 13 // D7
#define motorInterfaceType 1
bool ontime_delay = 1;
unsigned long currentMillis = 0;
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
void setup()
{
    Serial.begin(9600);
    Serial.println("test Software");
    pinMode(enablepin, OUTPUT);
    digitalWrite(enablepin, 0); // turn it on
    stepper.setCurrentPosition(0);
    stepper.moveTo(0);

    stepper.setMaxSpeed(100000);
    stepper.setSpeed(10000);
    stepper.setAcceleration(20000);
}

void loop()
{
    currentMillis = millis();
    if (currentMillis <= 15000)
    {
        stepper.move(-900000);
        Serial.println("anticlockwise");
    }
    else
    {
        if (ontime_delay)
        {
            stepper.setCurrentPosition(0);
            stepper.moveTo(0);
            digitalWrite(enablepin, 1); // turn it off
            Serial.println("Pause");
            delay(15000);
            ontime_delay = 0;
            digitalWrite(enablepin, 0); // turn it on
            stepper.move(900000);
        }
        else
        {
            stepper.move(900000);
            Serial.println("clockwise");
        }
    }

    stepper.setSpeed(1000);
    stepper.runSpeedToPosition();
}
