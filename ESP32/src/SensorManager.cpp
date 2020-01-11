#include <Arduino.h>
#include "SensorManager.h"

void initSensor()
{
        pinMode(pirPin, INPUT); //define the sensor
}

boolean isMovement()
{
    return digitalRead(pirPin)==1;
}