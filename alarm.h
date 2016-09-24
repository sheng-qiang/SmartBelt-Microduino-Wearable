#pragma once
#include "alarmHandler.h"
#include "trans.h"
#include "file.h"
#include "time.h"

#define ALARM_NUM 6

void alarm_load()
{
  for (int i = 0; i < ALARM_NUM; i++)
  {
    char c[] = "ALARM_0.txt";
    c[6] = i + 48;
    if (SD.exists(c))
    {
      Serial.print(c);
      Serial.println("  exists.");
      myFile = SD.open(c);
      alarms[i].openOrNot = (bool)(strToInt(myFile.readStringUntil('\n')));
      String modeStr = myFile.readStringUntil('\n');
      for (int j = 0; j < 7; j++)
        alarms[i].mode[j] = (bool)(modeStr[j] - 48);
      alarms[i].repeatOrNot = (bool)(strToInt(myFile.readStringUntil('\n')));
      alarms[i].h = (byte)(strToInt(myFile.readStringUntil('\n')));
      alarms[i].m = (byte)(strToInt(myFile.readStringUntil('\n')));
      alarms[i].repeatAllow = 1;
      if (alarms[i].repeatOrNot)
        alarms[i].repeatOnce = alarms[i].repeatTwice = 1;
      else
        alarms[i].repeatOnce = alarms[i].repeatTwice = 0;
//      Serial.print(alarms[i].openOrNot);
//      Serial.print(" ");
//      Serial.print(alarms[i].repeatOrNot);
//      Serial.print(" ");
//      Serial.print(modeStr);
//      Serial.print(" ");
//      Serial.print(alarms[i].h);
//      Serial.print(" ");
//      Serial.print(alarms[i].m);
//      Serial.print(" ");
//      Serial.print(alarms[i].repeatAllow);
//      Serial.print(" ");
//      Serial.print(alarms[i].repeatOnce);
//      Serial.print(" ");
//      Serial.println(alarms[i].repeatTwice);
      myFile.close();
    }
//    else
//    {
//      Serial.print(c);
//      Serial.println(" doesn't exist.");
//    }
  }
}

void alarm_check()
{
  for (int i = 0; i < ALARM_NUM; i++)
  {
    bool onAlarm = false;
    if (!alarms[i].openOrNot ||  !alarms[i].repeatAllow)
      continue;
    if (time_compare(alarms[i].h, alarms[i].m, weekday(now()) * alarms[i].mode[weekday(now()) - 1]))
    {
      onAlarm = true;
    }
    else if (alarms[i].repeatOrNot)
    {
      if (alarms[i].repeatOnce && time_compare(alarms[i].h, alarms[i].m, weekday(now()) * alarms[i].mode[weekday(now()) - 1], 1) )
      {
//        Serial.print(alarms[i].repeatOrNot);
//        Serial.println("  repeat detect 1");
        alarms[i].repeatOnce = 0;
        onAlarm = true;
        alarmHandler.reset_tone_now();
      }
      else if (alarms[i].repeatTwice && time_compare(alarms[i].h, alarms[i].m, weekday(now()) * alarms[i].mode[weekday(now()) - 1], 2) )
      {
//        Serial.println("  repeat detect 2");
        alarms[i].repeatTwice = 0;
        onAlarm = true;
        alarmHandler.reset_tone_now();
      }
    }
    if (onAlarm && alarmHandler.get_tone_now() != i)
    {
//      Serial.print(alarmHandler.get_tone_now());
//      Serial.print(" alarm tone No: ");
//      Serial.println(i);
      alarmHandler.start_alarm(i);
    }
  }
}

