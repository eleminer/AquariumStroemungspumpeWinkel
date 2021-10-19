#include <EEPROM.h>
int addr = 0;
void setup()
{
  EEPROM.begin(512);
  String defaultSettings = ",10,20,30,40,50,0,45,160,1,false,10:00,20:00,67,false,false,HE";
  for (int i = 0; i < defaultSettings.length(); i++)
  {
    EEPROM.write(0x0F + i, defaultSettings[i]);
  }
  EEPROM.commit();
}
void loop()
{
}
