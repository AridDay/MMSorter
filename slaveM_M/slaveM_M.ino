#include <AFMotor.h>
#include <Servo.h> 
#include <Wire.h>

#define DEGREES_CONSTANT 366
#define KILL_BYTE 0x00
#define BROWN_POS 0 //1
#define ORANGE_POS 60 //2
#define GREEN_POS 120 //3
#define RED_POS 180 //4
#define YELLOW_POS 240 //5
#define BLUE_POS 300 //6
static volatile int stepperPosition;
static volatile bool servoGoing=false;

AF_Stepper motor(200, 1);
Servo servo1,servo2;
String readIn="";
int currentPos=0;

void setup() {
  Serial.begin(9600);

  Wire.begin(2);
  Wire.onReceive(receiveEvent);
 
  motor.setSpeed(60);  //Stepper Speed 
  servo1.attach(9);
  servo2.attach(10);
  pinMode(2,INPUT);

  Serial.println("This is the M&M Slave");

}

void pathfind(int pos)
{
  if((pos-currentPos)<(360-pos+currentPos))
  {
    stepper((pos-currentPos),2);
  }else
  {
    stepper(((360-pos+currentPos)*-1),2);
  }
}

void stepper(double deg,int mode)
{
  Serial.print("DEG: ");
  Serial.println(deg);
  double s=(200.0/360.0)*deg;
  Serial.print("S: ");
  Serial.println(s);
  Serial.print("CP: ");
  Serial.println(currentPos);
  if(deg>=0)
  {
    currentPos+=deg;
  }else
  {
    currentPos= currentPos+deg;
    Serial.print("CP2: ");
    Serial.println(currentPos);
  }
  if(currentPos>=360)
      currentPos= currentPos-360;
  if(currentPos<0)
      currentPos+=360;
  Serial.print("CP3: ");
  Serial.println(currentPos);
 
  switch(mode)
  {
    case 1:
      if(deg>0)
        motor.step((int)s, FORWARD, SINGLE); 
      else{
      s=abs(s);
        motor.step((int)s, BACKWARD, SINGLE);}
    break;
    case 2:
      if(deg>0)
        motor.step((int)s, FORWARD, DOUBLE); 
      else{
      s=abs(s);
        motor.step((int)s, BACKWARD, DOUBLE);}
    break;
    case 3:
      if(deg>0)
        motor.step((int)s, FORWARD, INTERLEAVE); 
      else
      {
      s=abs(s);
        motor.step((int)s, BACKWARD, INTERLEAVE);
      }
    break;
    case 4:
      if(deg>0)
        motor.step((int)s, FORWARD, MICROSTEP); 
      else
      {
         s=abs(s);
        motor.step((int)s, BACKWARD, MICROSTEP);
      }
    break;
  }
}


//True(Forward) False(Backward)
void servo(int deg, int direc) //Experimental and Highly Inaccurate!
{
  if(direc==1)
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


void servoMag() /Accurate version of servo which uses Hall Effect Feedback
{
  servo1.write(0); Serial.println("Going");
  while(digitalRead(2)==HIGH){}
  servo1.write(90);Serial.println("Stopped");
}

void servoDeg(int deg)
{
  servo2.write(deg);
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
    
    case 0x1: //Stepper

      t1=Wire.read();
      Serial.println(t1);

      t2=Wire.read();
      Serial.println(t2);
      stepper(t1,t2);
    break;
    
    case 0x2: //Servo 1 (cont)
      servoGoing=true;
      Serial.println("servom");
    break;

    case 0x4:
      stepperPosition=BROWN_POS;
       Serial.println("brn");
     break;
    case 0x5:
      stepperPosition=ORANGE_POS;
      Serial.println("org");
    break;
    case 0x6:
      stepperPosition=GREEN_POS;
      Serial.println("grn");
    break;
    case 0x7:
      stepperPosition=RED_POS;
      Serial.println("red");
    break;
    case 0x8:
      stepperPosition=YELLOW_POS;
      Serial.println("yel");
    break;
    case 0x9:
      stepperPosition=BLUE_POS;
      Serial.println("blu");
    break;

     
  }
}


void loop() {
  if(stepperPosition!=-1)
  {
    pathfind(stepperPosition);
    stepperPosition=-1;
  }

  if(servoGoing)
  {
    servoMag();
    servoGoing=false;
  }

readIn=Serial.readString();
 if(readIn=="stepper"){
    Serial.println("Stepper");
    String t="";
    int t1;
    int t2;
    
    Serial.println("Degrees");
    Serial.flush();
    while(!Serial.available());
    t=Serial.readString();
    t1=t.toInt();
    Serial.println(t1);
    t="";

    Serial.println("Mode");
    Serial.flush();
    while(!Serial.available());
    t=Serial.readString();
    t2=t.toInt();
    Serial.println(t2);
    t="";

    stepper(t1,t2);
    
  }
  else if(readIn=="servo1"){
    Serial.println("Continuous Servo");
    String t;
    int t1;
    int t2;
    
    Serial.println("Degrees:");
    Serial.flush();
    while(!Serial.available());
    t=Serial.readString();
    t1=t.toInt();
    Serial.println(t1);
    t="";
    
    
    Serial.println("Direction:");
    Serial.flush();
    while(!Serial.available());
    t=Serial.readString();
    t2=t.toInt();
    Serial.println(t2);
    t="";

    servo(t1,t2);
  }
  else if(readIn=="servom"){
    Serial.println("Continuous Servo Mag");
    servoMag();
  }
  else if(readIn=="path"){
    Serial.println("Path");
    String t;
    int t1;
    
    Serial.println("Pos:");
    Serial.flush();
    while(!Serial.available());
    t=Serial.readString();
    t1=t.toInt();
    Serial.println(t1);
    t="";
    

    pathfind(t1);
  }
  
}
