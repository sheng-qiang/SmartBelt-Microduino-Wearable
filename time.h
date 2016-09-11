#pragma once
#include<Time.h>
#include "Wire.h"
#include <Rtc_Pcf8563.h>
Rtc_Pcf8563 rtc;

bool time_set(int _year, int _month, int _day, int _hour, int _min, int _sec, int _week)
{
  rtc.initClock();
  //day, weekday, month, century(1=1900, 0=2000), year(0-99)(need  - 16 when communicating)
  rtc.setDate(_day, _week, _month, 0, _year);
  rtc.setTime(_hour, _min, _sec);
  return true;
}

time_t time_get()
{
  rtc.formatDate();
  rtc.formatTime();
  setTime(rtc.getHour(), rtc.getMinute(), rtc.getSecond(), rtc.getDay(), rtc.getMonth(), rtc.getYear());
  return now();
}

void time_diff(int &d, int &h, int &m, int &s, time_t startTime, time_t endTime)
{
  int diff = endTime - startTime;
  s = diff % 60;
  m = diff / 60 % 60;
  h = diff / 3600 % 24;
  d = diff / 86400;
}

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

bool time_compare(int h, int m, int w)
{
  rtc.formatDate();
  rtc.formatTime();
  if (rtc.getHour() == h && rtc.getMinute() && rtc.getWeekday() == w)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

bool time_compare(int h, int m, int w, int deferMin)
{
  rtc.formatDate();
  rtc.formatTime();
  w = ( (w + (h + (m + deferMin) / 60) / 24) - 1) % 7 + 1;
  h = (h + (m + deferMin) / 60) % 24;
  m = (m + deferMin) % 60;
  if (rtc.getHour() == h && rtc.getMinute() && rtc.getWeekday() == w)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
