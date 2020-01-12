#include <Arduino.h>
#include "SensorManager.h"
#include "WebSocketManager.h"

static boolean reset = true;


void initSensor()
{
        pinMode(pirPin, INPUT); //define the sensor
}

boolean isMovement()
{
    return digitalRead(pirPin)==1;
}

void SensorLoop()
{
    if(isMovement())
    {
        if(!reset) {
            reset = true;
            Serial.printf("Mvt detected\n");

            /*We notify the server that we detect a movement*/
            if(isConnected())
            {
                sendText("Mvt");
            }
        }        
    }
    else if(reset)
    {
        Serial.printf("End Mvt detected\n");
        reset = false;
    }
    

}
