#pragma once
#include "SD.h"
#include "alarmHandler.h"
#include "trans.h"
#include "file.h"
#include <Rtc_Pcf8563.h>

#define ALARM_NUM 6
#define ALARM_TONE_MODE 2


struct AlarmParam
{
  bool openOrNot;
  byte h;
  byte m;
  bool mode[7];
  bool repeatOrNot;
} alarms[ALARM_NUM];

void alarm_load()
{

  for (int i = 0; i < ALARM_NUM; i++)
  {
    char c[] = "alarm_0.txt";
    c[6] = i + 48;
    if (SD.exists(c))
    {
      myFile = SD.open(c);
      alarms[i].openOrNot = (bool)(strToInt(myFile.readStringUntil('\n')));
      String modeStr = myFile.readStringUntil('\n');
      for (int j = 0; j < 7; j++)
      {
        alarms[i].mode[j] = (bool)(modeStr[j] - 48);
      }
      alarms[i].repeatOrNot = (bool)(strToInt(myFile.readStringUntil('\n')));
      alarms[i].h = (byte)(strToInt(myFile.readStringUntil('\n')));
      alarms[i].m = (byte)(strToInt(myFile.readStringUntil('\n')));
      myFile.close();
    }
  }
}

void alarm_check()
{
  bool onAlarm = 0;
  rtc.formatDate();
  for (int i = 0; i < ALARM_NUM; i++)
  {
    if (!alarms[i].openOrNot)
      continue;
    if (time_compare(alarms[i].h, alarms[i].m, rtc.getWeekday() * alarms[i].mode[rtc.getWeekday()]))
      onAlarm = 1;
    else if (alarms[i].repeatOrNot)
    {
      if (time_compare(alarms[i].h, alarms[i].m, rtc.getWeekday() * alarms[i].mode[rtc.getWeekday()], 10) ||
          time_compare(alarms[i].h, alarms[i].m, rtc.getWeekday() * alarms[i].mode[rtc.getWeekday()], 20))
        onAlarm = 1;
    }
    if (onAlarm)
    {
      alarmHandler.start_alarm(ALARM_TONE_MODE);
    }
  }
}
