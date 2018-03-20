#include <Wire.h>
#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //Color Sensor Integration && Gain settings
int yellowArr[]={120,101,57}; //Magic numbers to compare colors to average calibrated color
int blueArr[]={70,94,125};
int greenArr[]={80,126,91};
int redArr[]={172,80,80};
int brownArr[]={118,96,78};
int orangeArr[]={149,75,61};
String readIn="";


void setup() {
  pinMode(7,OUTPUT);
  digitalWrite(7,LOW);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("This is the M&M Master");
}


void(* resetFunc) (void) = 0; //Reset Everything Function

void morse(bool t)
{
  if(t==true)
  {
    digitalWrite(7,HIGH); //.
    Serial.print(".");
    delay(100);
    digitalWrite(7,LOW);
    delay(100);
  }else
  {
    digitalWrite(7,HIGH);//-
    Serial.print("-");
    delay(300);
    digitalWrite(7,LOW);
    delay(100);
  }
}

struct Color{
  int col[3];
};

struct Color getColor()
{
  struct Color ret;
  uint16_t clr, red, green, blue;

  digitalWrite(7,HIGH);

  delay(60);  // takes 50ms to read 

  
  
  tcs.getRawData(&red, &green, &blue, &clr);

  digitalWrite(7,LOW);
  

  // Conversion to hex code
  uint32_t sum = clr;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
//  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
//  Serial.println();
  
//    Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
    ret.col[0]=r;
    ret.col[1]=g;
    ret.col[2]=b;
    return ret;
}

void findColor(int acc){
//  Serial.println("Find Color");
        struct Color in=getColor();
      if(((brownArr[0]-acc)<in.col[0])&&(in.col[0]<(brownArr[0]+acc))
       &&((brownArr[1]-acc)<in.col[1])&&(in.col[1]<(brownArr[1]+acc))
       &&((brownArr[2]-acc)<in.col[2])&&(in.col[2]<(brownArr[2]+acc))
      )
      {
        Serial.println("Brown");
        sendI2C(0x4);
        delay(100);
      }else
      if(((orangeArr[0]-acc)<in.col[0])&&(in.col[0]<(orangeArr[0]+acc))
       &&((orangeArr[1]-acc)<in.col[1])&&(in.col[1]<(orangeArr[1]+acc))
       &&((orangeArr[2]-acc)<in.col[2])&&(in.col[2]<(orangeArr[2]+acc))
      ){
        Serial.println("orange");
        sendI2C(0x5);
        delay(100);
      }else
      if(((greenArr[0]-acc)<in.col[0])&&(in.col[0]<(greenArr[0]+acc))
       &&((greenArr[1]-acc)<in.col[1])&&(in.col[1]<(greenArr[1]+acc))
       &&((greenArr[2]-acc)<in.col[2])&&(in.col[2]<(greenArr[2]+acc))
      ){
        Serial.println("green");
        sendI2C(0x6);
        delay(100);
      }else
      if(((redArr[0]-acc)<in.col[0])&&(in.col[0]<(redArr[0]+acc))
       &&((redArr[1]-acc)<in.col[1])&&(in.col[1]<(redArr[1]+acc))
       &&((redArr[2]-acc)<in.col[2])&&(in.col[2]<(redArr[2]+acc))
      ){
        Serial.println("red");
        sendI2C(0x7);
        delay(100);
      }else
      if(((yellowArr[0]-acc-5)<in.col[0])&&(in.col[0]<(yellowArr[0]+acc+5))
       &&((yellowArr[1]-acc-5)<in.col[1])&&(in.col[1]<(yellowArr[1]+acc+5))
       &&((yellowArr[2]-acc-5)<in.col[2])&&(in.col[2]<(yellowArr[2]+acc+5))
      ){
        Serial.println("yellow");
        sendI2C(0x8);
        delay(100);
      }else
      if(((blueArr[0]-acc)<in.col[0])&&(in.col[0]<(blueArr[0]+acc))
       &&((blueArr[1]-acc)<in.col[1])&&(in.col[1]<(blueArr[1]+acc))
       &&((blueArr[2]-acc)<in.col[2])&&(in.col[2]<(blueArr[2]+acc))
      ){
        Serial.println("blue");
        sendI2C(0x9);
        delay(100);
      }
}

 void sendI2C(byte b)
{
  Wire.beginTransmission(2);
  Wire.write(b);
  Wire.endTransmission();
}

struct Color sampleAverageColor()
{
  int rgb[9][3];
  
  for(int i = 0; i<9; i++) //Takes 9 Samples
  {
    struct Color in = getColor();
    for(int j=0; j<3;j++)
      rgb[i][j]=in.col[j];
  }
  
  struct Color out;
  
  for(int i = 0; i<3;i++)
  {
    int av=0;
    for(int j = 0; j<9;j++)
      av+=rgb[j][i];

    out.col[i]=av/9;
  }
  
  return out;
}


void loop() {
  findColor(5);

    
  
  byte readIn=0x0;
  


  if (Serial.available() > 0) {
       readIn = Serial.read();
       Serial.println(readIn);
  }
//Console Tests
//  if(readIn=="reset"){
//      sendI2C(0xF);
//      Serial.println("Reset Imminent");
//      morse(true); morse(true); morse(true);      morse(false); morse(false); morse(false);     morse(true); morse(true); morse(true);
//      resetFunc();
//  }else if(readIn=="color")
//  {
//    for(int i=0; i<9;i++)
//    {
//    getColor();
//    delay(200);
//    }
//    
//
//  }else if(readIn=="servom")
//  {
//    sendI2C(0x2);
//    Serial.println("servom");
//  }
// 
//  else if(readIn=="brn"){
//    sendI2C(0x4);
//    Serial.println("brn");
//  }
//  else if(readIn=="org"){
//    sendI2C(0x5);
//    Serial.println("org");
//  }
//  else if(readIn=="grn"){
//    sendI2C(0x6);
//    Serial.println("grn");
//  }
//  else if(readIn=="red"){
//    sendI2C(0x7);
//    Serial.println("red");
//  }
//  else if(readIn=="yel"){
//    sendI2C(0x8);
//    Serial.println("yel");
//  }
//  else if(readIn=="blu"){
//    sendI2C(0x9);
//    Serial.println("blu");
//  }
//  else 
  
  if(readIn==49){
    
    Serial.println("Color:");
    Serial.flush();
    while(!Serial.available());
    String t=Serial.readString();
    Serial.println(t);

    struct Color in;
    if(t=="yellow")
    {
      Serial.println("YellowCal");
      in = sampleAverageColor();
      for(int i = 0; i<3; i++)
          yellowArr[i]=in.col[i];
          
    }else if(t=="blue")
    {
      Serial.println("BluCal");
      in = sampleAverageColor();
      for(int i = 0; i<3; i++)
          blueArr[i]=in.col[i];

    }else if(t=="green")
    {
      Serial.println("GRNCal");
      in = sampleAverageColor();
      for(int i = 0; i<3; i++)
          greenArr[i]=in.col[i];
    }
    else if(t=="red")
    {
      Serial.println("RedCal");
      in = sampleAverageColor();
      for(int i = 0; i<3; i++)
          redArr[i]=in.col[i];
    }
    else if(t=="brown")
    {
      Serial.println("BRNCal");
      in = sampleAverageColor();
      for(int i = 0; i<3; i++)
          brownArr[i]=in.col[i];
    }
    else if(t=="orange")
    {
      Serial.println("ORGcal");
      in = sampleAverageColor();
      for(int i = 0; i<3; i++)
          orangeArr[i]=in.col[i];
    }
   
    
    
    t="";
    Serial.println("calibrate");
  }
  
  else if(readIn==0x0){}
}



