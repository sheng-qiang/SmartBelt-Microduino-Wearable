#pragma once
#include "SD.h"
#include "trans.h"
#include "switch.h"
#include "time.h"
#include "file.h"
#include "alarmHandler.h"
#include "BLEhandler.h"

unsigned long sdtTime;
unsigned long sdtCacheTime;
bool sdtAlarmOrNot;
bool sdtContinualOrNot;
int sdtDuration = 0;

//0为坐，1为非坐
bool sdtState;

//不合并最小时间30
#define SDT_MERGE_INTERVAL 30 //30s is ok?
#define SDT_CACHE_INTERVAL 10

//在sdtShort中存储加速度信息
#define SDT_SHORT_LEN 10
double sdtShort[SDT_SHORT_LEN];
int sdtShortP;

double sdtShortAvg;
double sdtShortVar;

#define SDT_AVG_THR 1000
#define SDT_VAR_THR 50000

//在sdtLong数组中存储01序列，sdtShortAvg和sdtShortVar小于设定值时即为0，否则即为1
#define SDT_LONG_LEN 40
bool sdtLong[SDT_LONG_LEN];
int sdtLongP;

//定义当在长度为SDT_LONG_LEN0的数组中出现超过SDT_0_THR个0的时候sdtState转化为0
#define SDT_LONG_LEN0 5
//SDT_0_THR为SDT_LONG_LEN0的80%
#define SDT_0_THR 4
int sdtCount0;

//定义当在长度为SDT_LONG_LEN1的数组中出现超过SDT_0_THR个1的时候sdtState转化为1
int SDT_LONG_LEN1;
//SDT_LONG_LEN1随着久坐的时间长度而变化，最小值为SDT_LONG_LEN1_MIN，最大值为SDT_LONG_LEN
#define SDT_LONG_LEN1_MIN 8
//sdtThr1为SDT_LONG_LEN1的80%
int sdtThr1;
int sdtCount1;

//记录下每次久坐开始的时间
unsigned long sdtStartTime;
#define SDT_TIME_THR 7200
////#define SDT_TIME_THR 60


int sdtAlarmCtrl = 0;

//久坐检测部分LED灯的设定
//#define PIXEL_PIN    8    // Digital IO pin connected to the NeoPixels.
//#define PIXEL_COUNT  1
//Adafruit_NeoPixel switchLED = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void sdt_noi_update()
{
  int sdt_noi = 0;
  if (SD.exists("SST_NOI.TXT"))
  {
    myFile = SD.open("SDT_NOI.TXT");
    sdt_noi = strToInt(myFile.readStringUntil('\n'));
    myFile.close();
    SD.remove("SDT_NOI.TXT");
  }
  SD.open("SDT_NOI.TXT", FILE_WRITE);
  myFile.println(sdt_noi + 1);
  myFile.close();
}

void sdt_cache()
{
  if (!sdtState && now() - sdtCacheTime >= SDT_CACHE_INTERVAL)
  {
    //Serial.println("cache!");
    sdtCacheTime = now();
    SD.remove("SDT_CAC.TXT");
    myFile = SD.open("SDT_CAC.TXT", FILE_WRITE);
    myFile.println(sdtStartTime);
    myFile.println(now());
    myFile.close();
  }
}

void sdt_save(unsigned long eT, unsigned long sT)
{
  String s_temp;
  myFile = SD.open("SDT_INF.TXT", FILE_WRITE);
  if (eT == 0)
  {
    unsigned int params1[8] = {4, year(sT), month(sT), day(sT), hour(sT), minute(sT), 23, 59};
    bleHandler.encode(s_temp,params1);
  }
  else
  {
    unsigned int params2[8] = {4, year(eT), month(eT), day(eT), hour(sT), minute(sT), hour(eT), minute(eT)};
    bleHandler.encode(s_temp,params2);
  }
  myFile.println(s_temp);
  myFile.close();
  sdt_noi_update();
  SD.remove("SDT_CAC.TXT");
}

void sdt_init()
{
  //switchLED.begin();
  //switchLED.show(); // Initialize all pixels to 'off'
  sdtShortP = 0;
  sdtLongP = 0;
  for (int i = 0; i < SDT_LONG_LEN; i++)
  {
    sdtLong[i] = 1;
  }
  sdtShortAvg = 0;
  sdtShortVar = 0;
  sdtState = 1;
  SDT_LONG_LEN1 = SDT_LONG_LEN1_MIN;
  sdtCount1 = SDT_LONG_LEN1;
  sdtCount0 = 0;
  sdtTime = millis();
  sdtStartTime = now();
  //  SD卡读取当前配置
  myFile = SD.open("SDT_SET.TXT");
  sdtAlarmOrNot = (bool)(strToInt(myFile.readStringUntil('\n')));
  sdtContinualOrNot = (bool)(strToInt(myFile.readStringUntil('\n')));
  sdtDuration = strToInt(myFile.readStringUntil('\n'));//0为10min，1为20min，2为30min...
  myFile.close();
}

void sdt_alarm()
{
  if (!sdtState && sdtAlarmFlag && (now() - sdtStartTime) > SDT_TIME_THR + sdtAlarmCtrl * (sdtDuration + 1) * 600)
  {
    alarmHandler.start_alarm(7);
    //    Serial.print("Continual:");
    //    Serial.println(sdtContinualOrNot);
    if (sdtContinualOrNot)
    {
      sdtAlarmCtrl++;
    }
    else
    {
      sdtAlarmFlag = 0;
    }
  }
}

void sdt_judge()
{
  if (sdtAlarmOrNot)
  {
    sdt_alarm();
    sdtShort[sdtShortP] = abs(aaWorld.z) + abs(aaWorld.x) + abs(aaWorld.y);
    sdtShortAvg += sdtShort[sdtShortP];
    if (sdtShortP == SDT_SHORT_LEN - 1)
    {
      sdtShortAvg /= (double)SDT_SHORT_LEN;
      for (int i = 0; i < SDT_SHORT_LEN; i++)
      {
        sdtShortVar += (sdtShort[i] - sdtShortAvg) * (sdtShort[i] - sdtShortAvg);
      }
      sdtShortVar /= (double)SDT_SHORT_LEN;
      int s_p_temp = (sdtLongP - SDT_LONG_LEN0 + SDT_LONG_LEN) % SDT_LONG_LEN;
      if ((sdtShortAvg < SDT_AVG_THR) && (sdtShortVar < SDT_VAR_THR))
      {
        if (sdtLong[s_p_temp])
        {
          sdtCount0++;
        }
        sdtLong[sdtLongP] = 0;
      }
      else
      {
        if (!sdtLong[s_p_temp])
        {
          sdtCount0--;
        }
        sdtLong[sdtLongP] = 1;
      }
      //1-0
      if (sdtCount0 > SDT_0_THR && sdtState)
      {
        sdtState = 0; //坐
        sdtStartTime = now();
        sdtAlarmFlag = 1;
        sdtAlarmCtrl = 0;

        if (SD.exists("SDT_CAC.TXT"))
        {
          myFile = SD.open("SDT_CAC.TXT");
          unsigned long sdtCacheST = strToLong(myFile.readStringUntil('\n'));
          unsigned long sdtCacheET = strToLong(myFile.readStringUntil('\n'));
          myFile.close();
          SD.remove("SDT_CAC.TXT");
          if (sdtStartTime - sdtCacheET <= SDT_MERGE_INTERVAL)
          {
            myFile = SD.open("SDT_CAC.TXT", FILE_WRITE);
            myFile.println(sdtCacheST);
            myFile.println(sdtStartTime);
            myFile.close();
            //改变起始时间
            sdtStartTime = sdtCacheST;
          }
          else
          {
            sdt_save(sdtCacheET, sdtCacheST);
            //写入新的缓存数据
            myFile = SD.open("SDT_CAC.TXT", FILE_WRITE);
            myFile.println(sdtStartTime);
            myFile.println(now());
            myFile.close();
          }
        }
      }
      //根据坐的时间，计算0-1的阈值，坐的时间越长，则阈值越大，即越不容易退出坐的状态
      if (!sdtState)
      {
        if (now() - sdtStartTime < SDT_TIME_THR)
        {
          //最小值为SDT_LONG_LEN1_MIN，最大值为SDT_LONG_LEN
          SDT_LONG_LEN1 = SDT_LONG_LEN1_MIN + (now() - sdtStartTime) * (SDT_LONG_LEN - SDT_LONG_LEN1_MIN) / SDT_TIME_THR;
        }
        else
        {
          SDT_LONG_LEN1 = SDT_LONG_LEN;
        }
      }
      //计算最近的SDT_LONG_LEN1中有多少个1
      sdtCount1 = 0;
      for (int i = 0; i < SDT_LONG_LEN1; i++)
      {
        sdtCount1 += (int)sdtLong[(sdtLongP - i + SDT_LONG_LEN) % SDT_LONG_LEN];
      }
      sdtThr1 = SDT_LONG_LEN1 * 0.8;
      //0-1
      if ((sdtCount1 > sdtThr1) && (!sdtState))
      {
        sdtState = 1; //站
        //此处应存储信息
        sdt_save(now(), sdtStartTime);
      }
      sdtLongP = (sdtLongP + 1) % SDT_LONG_LEN;
      //      Serial.print("average:");
      //      Serial.print(sdtShortAvg);
      //      Serial.print("  sdtShortVar:");
      //      Serial.print(sdtShortVar);
      //      Serial.print("  sdtStartTime=");
      //      Serial.print(sdtStartTime);
      //      Serial.print("  ");
      //      Serial.print(sdtLong[sdtLongP - 1]);
      //      Serial.print("  count0=");
      //      Serial.print(sdtCount0);
      //      Serial.print("  count1=");
      //      Serial.print(sdtCount1);
      //      Serial.print("  len=");
      //      Serial.print(SDT_LONG_LEN1);
      //      Serial.print("  state:");
      //      Serial.println(sdtState);
      //      Serial.print("AlarmFlag:");
      //      Serial.print(sdtAlarmFlag);
      //      Serial.print("  dtime:");
      //      Serial.print(now() - sdtStartTime);
      //      Serial.print("  Ctrl:");
      //      Serial.print(sdtAlarmCtrl);
      //      Serial.print("  thr");
      //      Serial.print(SDT_TIME_THR + sdtAlarmCtrl * (sdtDuration + 1) * 600);
      //      Serial.print("  now:");
      //      Serial.println(now());
      //LED灯部分
      if (sdtState)
      {
        switchLED.setPixelColor(2, switchLED.Color(0, 0, 0));
        switchLED.show();
      }
      else
      {
        switchLED.setPixelColor(2, switchLED.Color(0, 0, 5));
        switchLED.show();
      }
    }
    sdtShortP = (sdtShortP + 1) % SDT_SHORT_LEN;
    sdtTime = millis();
  }
}
