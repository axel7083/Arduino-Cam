#include <Arduino.h>
#include <string.h>
#include <Servo.h>
#include "ServoManager.h"

Servo servo1;

void initServo()
{
    servo1.attach(servoPin); //define the servo pin
    servo1.write(initial_degree); //set initial degree of servo to 0
}

/*Return the current degree of the servo*/
int getRotate()
{
  return servo1.read();
}

/*degree varies between 0 and 180*/
void setRotate(int degree, int delay_u)
{
  if(delay > 0 && degree >= 0 && degree <= 180)
  {
    int actual = servo1.read();

    if(actual > degree)
    {
      float gap = (float) delay_u / (actual - degree);

      for(int i = actual; i > degree; i--)
      {
        servo1.write(i);
        delay(gap);
      }
    }
    else
    {
      if(actual < degree)
      {
        float gap = (float) delay_u / (degree - actual);

        for(int i = actual; i < degree; i++)
        {
          servo1.write(i);
          delay(gap);
        }
      }
      else
      {
        Serial.print("No movements needed (servo motor is already at ");
        Serial.print(degree);
        Serial.print(" degrees");
      }
    }
  }
  else
  {
    if(delay <= 0)
    {
      Serial.print("parameter 'delay' out of range (TIP : delay > 0)");
    }
    else
    {
      if(degree < 0)
      {
        Serial.print("parameter 'degree' out of range (TIP : degree >= 0");
      }
      else
      {
        Serial.print("parameter 'degree' out of range (TIP degree <= 180)");
      }
    }
  }
}

