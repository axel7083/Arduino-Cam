#include <Arduino.h>
#include <WiFi.h>

#include "SensorManager.h"
#include "WebSocketManager.h"
#include "CamManager.h"
#include "ServoManager.h"
#include "Output.h"


//Station mode you should put your ssid and password
const char* ssid = "Patates"; // Put your SSID here
const char* password = "eqndnrjg"; // Put your PASSWORD here


void setup()
{

    Serial.begin(500000);

    /*Init the ArduCam*/
    initCam();

    /*Init Servo*/
    initServo();

    /*Init the mouvement sensor*/
    initSensor();

    /*Init the websocket*/
    initWebSocket();

    //set the CS as an output:
    pinMode(CS, OUTPUT);

    /*Init the led output*/
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    
    /*Setup the led power*/
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);

    /*Setup the wifi station*/
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println(F("WiFi connected"));
    Serial.println("");
    Serial.println(WiFi.localIP());

    Serial.println();
    Serial.println();
    Serial.print(F("Connecting to "));
    Serial.println(ssid);

//myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //enable low power
//delay(300000); //5minutes
//myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //disable low power

}



void loop() {
  loopWebSocket();

  /*if(isMovement())
  {
    Serial.printf("MVT DETECTED");
  }*/
}
