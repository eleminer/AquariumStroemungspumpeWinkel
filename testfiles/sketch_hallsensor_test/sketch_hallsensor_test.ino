#include <Wire.h>
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;
void setup()
{
    ads.setGain(GAIN_ONE);
    ads.begin();
    Serial.begin(9600);
}
void loop()
{
    Serial.println("Anschlag Null:"+String(ads.readADC_SingleEnded(1))+"   Anschlag Ende:"+String(ads.readADC_SingleEnded(2)));
}