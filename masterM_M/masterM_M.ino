#include <Wire.h>
#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

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

void getColor()
{
  
  uint16_t r, g, b, c, colorTemp, lux;
  
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  
  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  
}

void loop() {
  readIn=Serial.readString();

//Console Tests
  if(readIn=="reset"){
      sendI2C(0xF);
      Serial.println("Reset Imminent");
      morse(true); morse(true); morse(true);      morse(false); morse(false); morse(false);     morse(true); morse(true); morse(true);
      resetFunc();
  }else if(readIn=="color")// Have to do 3 times, or else color isn't accurate. No clue why.
  {
    digitalWrite(7,HIGH);
    getColor();
    getColor();
    getColor();
    digitalWrite(7,LOW);
  }
  else if(readIn==""){}
}

void sendI2C(byte b)
{
  Wire.beginTransmission(2);
  Wire.write(b);
  Wire.write(4);
  Wire.endTransmission();
}

