#pragma once
#include <Adafruit_NeoPixel.h>

#define PIXEL_SWITCH    6
#define PIXEL_COUNT  3

#define BUTTON A0

Adafruit_NeoPixel switchLED = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_SWITCH, NEO_GRB + NEO_KHZ800);

#define BUTTONLEN 5
bool button[BUTTONLEN];
int button_p;
unsigned long buttonTime;
bool buttonNow;
bool buttonLast;
bool switchStateMode1;
bool switchStateMode1b;
bool switchStateMode2;
unsigned long switchTime1;
unsigned long switchTime2;
unsigned long switchBeginTime1;
unsigned long switchBeginTime2;

bool switch_init()
{
  switchLED.begin();
  switchLED.show();
  buttonNow = 1;
  buttonLast = 1;
  for (int i=0;i<BUTTONLEN;i++)
  {
    button[i]=1;
  }
  button_p=0;
  switchStateMode1 = LOW;
  switchStateMode1b = LOW;
  switchStateMode2 = LOW;
  switchTime1 = 0;
  switchTime2 = 0;
  switchBeginTime1 = 0;
  switchBeginTime2 = 0;
  buttonTime = millis();
}

void switch_judge()
{
  /*
   * 判断开关状态,长按3s可更改开关状态
   * switchStateMode1
   */
  switchStateMode1b = switchStateMode1;
  buttonLast = buttonNow;
  button[button_p]=(boolean)(analogRead(BUTTON)>500);
  button_p=(button_p+1)%BUTTONLEN;
//  Serial.print("p:");
//  Serial.print(button_p);
//  Serial.print("  now:");
//  Serial.print(button[button_p]);
  int sum=0;
  for (int i=0;i<BUTTONLEN;i++)
  {
    sum+=(int)button[i];
//    Serial.print("  ");
//    Serial.print(button[i]);
  }
  buttonNow=(boolean)(sum>(BUTTONLEN/2));
  //buttonNow=analogRead(BUTTON);
  if (buttonLast && !buttonNow)
  {
    switchBeginTime1 = millis();
  }
//    Serial.print("  buttonLast:");
//  Serial.print(buttonLast);
//  Serial.print("  buttonNow:");
//  Serial.print(buttonNow);
//  Serial.print(" begintime");
//  Serial.println(switchBeginTime2);
  if (!buttonNow && switchBeginTime1)
  {
    switchTime1 = millis() - switchBeginTime1;
    if (switchTime1 > 3000)
    {
      switchStateMode1 = !switchStateMode1;
      switchBeginTime1 = 0;
    }
  }
  if (buttonNow)
  {
    switchBeginTime1 = 0;
  }
    /*
   * 判断开关状态
   * 轻按可更改开关状态
   * switchStateMode2
   */
   if (buttonLast && !buttonNow)
  {
    switchBeginTime2=millis();
  }
  if(buttonNow&&switchBeginTime2)
  {
    switchTime2=millis()-switchBeginTime2;
    if(switchTime2<500)
    {
      switchStateMode2 =1; //!switchStateMode2;
    }
    switchBeginTime2=0;
  }
  if(switchBeginTime2&&((millis()-switchBeginTime2)>=500))
  {
    switchBeginTime2=0;
  }
  buttonTime=millis();
}
