#pragma once
#include "SD.h"
#include "time.h"
#include "BLEhandler.h"

#define STEP_INTERVAL_MIN 300
#define STEP_INTERVAL_MAX 2000
#define STEP_CACHE_INTERVAL 10000
#define STEP_CACHE_RECOVER_INTERVAL 5

int origStep[5], smoothStep[3];
int origStepPos = 0, smoothStepPos = 0, todayPos = 0;
int pkf[2], pkCount, thr[2], hour_f;
bool pkfFlag = 0;
unsigned long pkfTime[2], pkdTime;
int tempHour = -1, tempMinute = -1;

int stepCacheTime;
bool stepCacheOn = false;

bool stepAlarmOrNot;
int stepTarget;
int stepToday;

void step_save(int y, int m, int d, int sH, int sM, int eH, int eM, int steps)
{
  int params[9] = {4, y, m, d, sH, sM, eH, eM, steps};
  String s =  bleHandler.encode(params);
  myFile = SD.open("step.txt", FILE_WRITE);
  myFile.println(s);
  myFile.close();
  SD.remove("step_totalToday.txt");
  myFile = SD.open("step_totalToday.txt", FILE_WRITE);
  myFile.println(steps + stepToday);
  myFile.close();
  //to initial state
  pkCount = 0;
  pkf[0] = pkf[1] = 0;
  pkfFlag = 0;
  stepCacheOn = false;
  SD.remove("step_cache.txt");
}

void step_init()
{
  myFile = SD.open("step_cache.txt");
  if (now() - strToLong(myFile.readStringUntil('\n')) <= STEP_CACHE_RECOVER_INTERVAL )
  {
    
  }
  else
  {
    pkfTime[1] = millis();
    pkfFlag = 0;
    thr[0] = thr[1] = 0;
    stepAlarmOrNot = 1;
  }
};

void step_cache()
{
  if (stepCacheOn && millis() - stepCacheTime >= STEP_CACHE_INTERVAL)
  {
    stepCacheTime = millis();
    SD.remove("step_cache.txt");
    myFile = SD.open("step_cache.txt", FILE_WRITE);
    myFile.println(now());
    myFile.println(tempHour);
    myFile.println(tempMinute);
    myFile.println(pkCount);
    myFile.println(pkf[0]);
    myFile.println(pkf[1]);
    myFile.println(pkFlag);
    myFile.println(origStep[(origStepPos - 1) % 5]);
    myFile.println(origStep[(origStepPos) % 5]);
    myFile.println(smoothStep[(smoothStepPos - 1) % 3]);
    myFile.println(smoothStep[smoothStepPos % 3]);
  }
}


void step_detect()
{
  if (pkCount == 3 && tempHour == -1 && tempMinute == -1)
  {
    rtc.getTime();
    tempHour = rtc.getHour();
    tempMinute = rtc.getMinute();
  }
  origStep[origStepPos] = aaWorld.z;
  smoothStep[smoothStepPos] = (origStep[origStepPos] + origStep[(origStepPos - 1) % 5] + origStep[(origStepPos - 2) % 5]) / 3;
  if (pkf[0] != 0) thr[0] = 0.25 * pkf[0] + 1000;
  if (pkf[1] != 0) thr[1] = 0.25 * pkf[1] + 1000;

  //跨天判断!!!!!
  //if (rtc.getHour() == 0 && hour_f == 23)
  if (smoothStep[(smoothStepPos - 1) % 3] < 8000 && smoothStep[(smoothStepPos - 1) % 3] > smoothStep[(smoothStepPos - 2) % 3]  && smoothStep[(smoothStepPos - 1) % 3] > smoothStep[smoothStepPos])
  {
    pkdTime = millis() - pkfTime[!pkfFlag];
    if (pkf[0] != 0)
    {
      if (pkdTime < STEP_INTERVAL_MIN)
      {
        if (smoothStep[(smoothStepPos - 1) % 3] > thr[!pkfFlag] && smoothStep[(smoothStepPos - 1) % 3] > pkf[!pkfFlag])
        {
          pkf[!pkfFlag] = smoothStep[(smoothStepPos - 1) % 3];
          pkfTime[!pkfFlag] = millis();
        }
      }
      else
      {
        if (smoothStep[(smoothStepPos - 1) % 3] > thr[pkfFlag])
        {
          if (pkdTime <= STEP_INTERVAL_MAX)
          {
            pkf[pkfFlag] = smoothStep[(smoothStepPos - 1) % 3];
            pkCount++;
            pkfTime[pkfFlag] = millis();
            pkfFlag = !pkfFlag;
          }
        }
        else
        {
          //for communicating, the YEAR has been modified
          rtc.formatTime();
          step_save(rtc.getYear() - 16, rtc.getMonth(), rtc.getDay(), tempHour, tempMinute,
                    (rtc.getHour() * 60 + rtc.getMinute() - pkdTime / 60000) / 60 % 24 , (rtc.getHour() * 60 + rtc.getMinute() - pkdTime / 60000) % 60 , pkCount);

        }
      }
    }
    else
    {
      //the first peak appeared
      if (smoothStep[(smoothStepPos - 1) % 3] > thr[pkfFlag])
      {
        pkf[0] = smoothStep[(smoothStepPos - 1) % 3];
        pkCount = 1;
        pkfTime[0] = millis();
        pkfFlag = 1;
        //record startTime
        rtc.formatTime();
        tempHour = rtc.getHour();
        tempMinute = rtc.getMinute();
        stepCacheTime = millis;
        stepCacheOn = true;
      }
    }
  }
  origStepPos = (origStepPos + 1) % 100;
  smoothStepPos = (smoothStepPos + 1) % 3;
}

