#include <Arduino.h>

void setup()
{
    Serial1.begin(38400);
    Serial.begin(9600);
}
 
void loop()
{
    if (Serial1.available())
        Serial.write(Serial1.read());
    
    if (Serial.available())
        Serial1.write(Serial.read());
}