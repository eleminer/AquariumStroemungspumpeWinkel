#include <EEPROM.h>
int addr = 0;
void setup()
{
  EEPROM.begin(512);
  String defaultSettings = "1:10?2:20?3:30?4:40?5:50?d:10?u:160?s:1?o:1E";
  for(int i=0;i<defaultSettings.length();i++)
  {
    EEPROM.write(0x0F+i, defaultSettings[i]);
  }
  EEPROM.commit();
}
 
void loop()
{   
}
