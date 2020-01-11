#include <Arduino.h>
#include <WebSocketsClient.h>

void initWebSocket();
void loopWebSocket();
void sendData(uint8_t* data,uint32_t length);
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
int extract(char * value,size_t length);