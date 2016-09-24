#pragma once
#include <Time.h>
#include "Wire.h"
#include <Rtc_Pcf8563.h>
Rtc_Pcf8563 rtc;

unsigned long timeBefore;
unsigned long newDayTime;

bool time_compare(int h, int m)
{
  rtc.formatDate();
  rtc.formatTime();
  if (rtc.getHour() == h && rtc.getMinute())
  {
    return 1;
  }
  else
  {
    return 0;
  }
}


//bool time_compare(int h, int m, int w)
//{
////  rtc.formatDate();
////  rtc.formatTime();
////  Serial.print(h);
////  Serial.print(":");
////  Serial.print(m);
////  Serial.print(" ");
////  Serial.print(rtc.getHour());
////  Serial.print(":");
////  Serial.println(rtc.getMinute());
////  Serial.print(" ");
////  Serial.print(rtc.getWeekday());
//
//  if (rtc.getHour() == h && rtc.getMinute() == m && rtc.getWeekday() == w)
//  {
//    return 1;
//  }
//  else
//  {
//    return 0;
//  }
//}

bool time_compare(int h, int m, int w, int deferMin = 0)
{
  rtc.formatDate();
  rtc.formatTime();
  w = ( (w + (h + (m + deferMin) / 60) / 24) - 1) % 7 + 1;
  h = (h + (m + deferMin) / 60) % 24;
  m = (m + deferMin) % 60;
  //  Serial.print(h);
  //  Serial.print(":");
  //  Serial.print(m);
  //  Serial.print("  ");
  //  Serial.print(rtc.getHour());
  //  Serial.print(":");
  //  Serial.println(rtc.getMinute());
  if (rtc.getHour() == h && rtc.getMinute() == m) //&& rtc.getWeekday() == w)
  {
    //    Serial.println("REPEAT NOW !");
    //    Serial.print(h);
    //    Serial.print(":");
    //    Serial.print(m);
    //    Serial.print(" ");
    //    Serial.print(rtc.getHour());
    //    Serial.print(":");
    //    Serial.println(rtc.getMinute());
    //    Serial.print(" ");
    //    Serial.print(rtc.getWeekday());
    return 1;
  }
  else
  {
    return 0;
  }
}


void time_t_init()
{
  rtc.formatDate();
  rtc.formatTime();
  setTime(rtc.getHour(), rtc.getMinute(), rtc.getSecond(), rtc.getDay(), rtc.getMonth(), rtc.getYear());
}

void time_rtc_init(int _year, int _month, int _day, int _week, int _hour, int _min, int _sec)
{
  rtc.initClock();
  //day, weekday, month, century(1=1900, 0=2000), year(0-99)
  rtc.setDate(_day, _week, _month, 0, _year);
  rtc.setTime(_hour, _min, _sec);
}
