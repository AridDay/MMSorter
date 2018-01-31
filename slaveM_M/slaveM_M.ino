#include <AFMotor.h>
#include <Servo.h> 
#include <Wire.h>

#define DEGREES_CONSTANT 366
#define KILL_BYTE 0x00

AF_Stepper motor(200, 1);
Servo servo1;
String readIn="";

void setup() {
  Serial.begin(9600);

  Wire.begin(2);
  Wire.onReceive(receiveEvent);
 
  motor.setSpeed(10);  // 10 rpm   
  servo1.attach(9);

  Serial.println("This is the M&M Slave");

}

void stepper(int deg,int mode)
{
  int s=(270*deg)/360;
  switch(mode)
  {
    case 1:
      if(deg>0)
        motor.step(s, FORWARD, SINGLE); 
      else if(deg<0)
        motor.step(s, BACKWARD, SINGLE);
    break;
    case 2:
      if(deg>0)
        motor.step(s, FORWARD, DOUBLE); 
      else if(deg<0)
        motor.step(s, BACKWARD, DOUBLE);
    break;
    case 3:
      if(deg>0)
        motor.step(s, FORWARD, INTERLEAVE); 
      else if(deg<0)
        motor.step(s, BACKWARD, INTERLEAVE);
    break;
    case 4:
      if(deg>0)
        motor.step(s, FORWARD, MICROSTEP); 
      else if(deg<0)
        motor.step(s, BACKWARD, MICROSTEP);
    break;
  }
}


//True(Forward) False(Backward)
void servo(int deg, bool direc) //Experimental and Highly Inaccurate!!!! ADD ROTARY ENCODER
{
  if(direc)
  {
    servo1.write(0);
    int t=(deg/90)*DEGREES_CONSTANT;
    delay(t);
    servo1.write(90);
  }
  else
  {
    servo1.write(180);
    int t=(deg/90)*DEGREES_CONSTANT;
    delay(t);
    servo1.write(90);
  }
}

void(* resetFunc) (void) = 0; //Reset Everything Function

void receiveEvent(int howMany) {

  byte x = Wire.read();
  Serial.println(x);
  int t1,t2;
  bool t3;

  switch (x)
  {
    case 0xF:
    Serial.println("Reset Imminent");
    delay(1000);
    resetFunc();
    break;
    
    case 0x1:
      t1=Wire.read();
      t2=Wire.read();
      stepper(t1,t2);
    break;
    
    case 0x2:
      t1=Wire.read();
      t3=Wire.read();
      servo(t1,t3);
    break;
  }
}



void loop() {

  delay(100);
}
