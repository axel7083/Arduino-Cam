#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>

#include "memorysaver.h"
#include "CamManager.h"
#include "WebSocketManager.h"

static ArduCAM myCAM(OV5642, CS);

static uint8_t buffer[30000] = { 0xFF };
static uint32_t length = 0;
uint8_t temp = 0, temp_last = 0;
bool is_header = false;
int i = 0;

void initCam()
{
    uint8_t vid, pid;
    uint8_t temp;

    Wire.begin();

    Serial.println(F("ArduCAM Start!"));

    // initialize SPI:
    SPI.begin();
    SPI.setFrequency(4000000); //4MHz

    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55) {
        Serial.println(F("SPI1 interface Error!"));
        while (1)
            ;
    }

    //Check if the camera module type is OV5642
    myCAM.wrSensorReg16_8(0xff, 0x01);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
    if ((vid != 0x56) || (pid != 0x42)) {
        Serial.println(F("Can't find OV5642 module! But that's not a big deal... Because it work ahah"));
    }
    else
        Serial.println(F("OV5642 detected."));

    //Change to JPEG capture mode and initialize the OV2640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK); //VSYNC is active HIGH
    myCAM.OV5642_set_JPEG_size(OV5642_320x240);

    myCAM.clear_fifo_flag();
     
    myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //enable low power
}


void start_capture()
{
    myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //disable low power
    myCAM.clear_fifo_flag();
    myCAM.start_capture();
}

void camCapture(ArduCAM myCAM)
{
 
    uint32_t len = myCAM.read_fifo_length();

    length = len;

    if (len >= MAX_FIFO_SIZE) //8M
    {
        Serial.println(F("Over size."));
    }
    else if (len == 0) //0 kb
    {
        Serial.println(F("Size is 0."));
    }
    else
    {
        Serial.printf("Size is %d",len);
    }
    
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();


    while (len--) {
        temp_last = temp;
        temp = SPI.transfer(0x00);
        //Read JPEG data from FIFO
        if ((temp == 0xD9) && (temp_last == 0xFF)) //If find the end ,break while,
        {
            buffer[i++] = temp; //save the last  0XD9
            is_header = false;
            i = 0;
            myCAM.CS_HIGH();
            break;
        }
        if (is_header == true) {
            //Write image data to buffer if not full
            buffer[i++] = temp;

        }
        else if ((temp == 0xD8) & (temp_last == 0xFF)) {
            is_header = true;
            buffer[i++] = temp_last;
            buffer[i++] = temp;
        }
    }

  sendData(&buffer[0],length);

}

void serverCapture()
{
    Serial.println(F(" --- "));
    Serial.println(F("CAM Capturing"));
    start_capture();

    int total_time = 0;

    total_time = millis();
    while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        ;
    total_time = millis() - total_time;
    Serial.print(F("capture total_time used (in miliseconds):"));
    Serial.println(total_time, DEC);

    total_time = 0;

    Serial.println(F("CAM Capture Done."));
    total_time = millis();
    camCapture(myCAM);

    //send data to serial port
    //Serial.println("[");
    //Serial.write(&buffer[0], length);
    //Serial.println("]");

    total_time = millis() - total_time;
    Serial.print(F("send total_time used (in miliseconds):"));
    Serial.println(total_time, DEC);

    Serial.println(F("CAM send Done."));


    Serial.println(F(" --- "));
}
