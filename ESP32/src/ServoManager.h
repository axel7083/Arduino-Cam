/*Servo header*/
#define initial_degree 0 //define the initial degree orientation of the servo
#define servoPin 4

void initServo();
int getRotate();
void setRotate(int degree, int delay_u);