// ArduCAM Mini demo (C)2017 Lee
// Web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with ArduCAM ESP32 2MP/5MP camera.
// This demo was made for ArduCAM ESP32 2MP/5MP Camera.
// It can take photo and send to the Web.
// It can take photo continuously as video streaming and send to the Web.
// The demo sketch will do the following tasks:
// 1. Set the camera to JPEG output mode.
// 2. if server receives "GET /capture",it can take photo and send to the Web.
// 3.if server receives "GET /stream",it can take photo continuously as video
//streaming and send to the Web.

// This program requires the ArduCAM V4.0.0 (or later) library and ArduCAM ESP32 2MP/5MP camera
// and use Arduino IDE 1.8.1 compiler or above


#include <WiFi.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

#include <WebSocketsClient.h>

WebSocketsClient webSocket;

#define USE_SERIAL Serial
#define GPIO_PWDN_MASK			0x02  //0 = Sensor normal operation, 	1 = Sensor 

// set GPIO17 as the slave select :
const int CS = 15;
bool connected = false;

ArduCAM myCAM(OV5642, CS);


//Station mode you should put your ssid and password
const char* ssid = "Patates"; // Put your SSID here
const char* password = "eqndnrjg"; // Put your PASSWORD here

static uint8_t buffer[30000] = { 0xFF };
static uint32_t length = 0;
uint8_t temp = 0, temp_last = 0;
int i = 0;
bool is_header = false;

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


    i = 0;
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

    webSocket.sendBIN(&buffer[0], length);

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

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");

			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			webSocket.sendTXT("ESP32");
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			serverCapture();
            

			break;
		case WStype_BIN:
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}


void setup()
{
    uint8_t vid, pid;
    uint8_t temp;
    //set the CS as an output:
    pinMode(CS, OUTPUT);

    Wire.begin();

    Serial.begin(500000);
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
        Serial.println(F("Can't find OV5642 module!"));
    }
    else
        Serial.println(F("OV5642 detected."));

    //Change to JPEG capture mode and initialize the OV2640 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK); //VSYNC is active HIGH
    myCAM.OV5640_set_JPEG_size(OV5642_320x240);

    myCAM.clear_fifo_flag();


        // Connect to WiFi network
        Serial.println();
        Serial.println();
        Serial.print(F("Connecting to "));
        Serial.println(ssid);

        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        int total_time = millis();

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(F("."));
        }
        Serial.println(F("WiFi connected"));
        Serial.println("");
        Serial.println(WiFi.localIP());

    // server address, port and URL
	webSocket.begin("192.168.43.211", 81, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
    
    myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //enable low power
//myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //enable low power
//delay(300000); //5minutes
//myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //disable low power
}



void loop() {
webSocket.loop();
}