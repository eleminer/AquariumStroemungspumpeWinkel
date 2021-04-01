#include <EEPROM.h>
int addr = 0;
char eeprom[1000] = "";
char *pointer = eeprom;

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup()
{
  EEPROM.begin(512);
  Serial.begin(9600);
  int i = 0;
  while ((char)EEPROM.read(i) != 'E')
  {
    Serial.print((char)EEPROM.read(i));
    *pointer = (char)EEPROM.read(i);
    pointer++;
    i++;
  }
  Serial.println(eeprom);
}

void loop()
{
  String button1 = getValue(eeprom, ',', 1);
  Serial.println("eins:" + button1);
}
