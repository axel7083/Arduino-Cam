#include <Arduino.h>
#include <WebSocketsClient.h>

#include "ServoManager.h"
#include "WebSocketManager.h"
#include "CamManager.h"
#include "Output.h"

static WebSocketsClient webSocket;
static boolean connected = false;

boolean isConnected()
{
    return connected;
}


void initWebSocket()

{
    // server address, port and URL
	webSocket.begin("192.168.43.94", 81, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
}

void loopWebSocket()
{
    webSocket.loop();
}

void sendData(uint8_t* data,uint32_t length)
{
    webSocket.sendBIN(data, length);
}

void sendText(char* data)
{
    webSocket.sendTXT(data);
}

int extract(char * value,size_t length)
{
    Serial.printf("value: %s\n",value);
    uint8_t deg = 0;
    for(int i = 0; i < length-4; i ++) 
    {
    deg *=10;
    deg += value[i] - '0';
    }
    return deg;  
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    char * payloadChar =(char *) payload;
    if(length>3)
    {
        Serial.printf(">3!\n");
        payloadChar[3] = '\0';
    }
    
    switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");

            connected= false;

            digitalWrite(ledRed, HIGH);
            digitalWrite(ledGreen, LOW);
      
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
            connected = true;
            
            digitalWrite(ledRed, LOW);
            digitalWrite(ledGreen, HIGH);

			// send message to server when Connected
			webSocket.sendTXT("ESP32");
			break;
		case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payloadChar);

            if (strcmp(payloadChar, "Req") == 0) {
                Serial.printf("Req");
                serverCapture();
            }
            /*We are controlling the rotation from here*/
            else if (strcmp(payloadChar, "Rot") == 0) {
                Serial.printf("Rot\n");

                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, LOW);
                uint8_t deg = extract ( (char *) payload + 4 ,length);
                Serial.printf("deg : %d\n",deg);
                setRotate(deg,100);
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, HIGH);


            }
            /*We are controlling the led from here*/
            else if (strcmp(payloadChar, "Led") == 0) {
                uint8_t intensity = extract ( (char *) payload + 4 ,length);
                Serial.printf("intensity : %d\n",intensity);
                digitalWrite(ledGreen, intensity);
                //digitalWrite(ledRed, intensity);
            }
            else if (strcmp(payloadChar, "Reb") == 0) {
                Serial.printf("Reb\nRESTARTING...");
                ESP.restart();
            }
            else
            {
                Serial.printf("Default");
            }
            
            

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