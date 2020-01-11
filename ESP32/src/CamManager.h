#include <ArduCAM.h>

// set GPIO17 as the slave select :
#define CS 15
//0 = Sensor normal operation, 	1 = Sensor  (cam)
#define GPIO_PWDN_MASK			0x02  

void initCam();
void start_capture();
void camCapture(ArduCAM myCAM);
void serverCapture();