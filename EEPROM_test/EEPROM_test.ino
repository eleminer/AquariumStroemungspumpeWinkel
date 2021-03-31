#include <EEPROM.h>
int addr = 0;
 
void setup()
{
EEPROM.begin(512);
Serial.begin(9600);
int i=0;
while((char)EEPROM.read(i)!='E')
{
  Serial.print((char)EEPROM.read(i));
  i++;
}

}
 
void loop()
{   
}
