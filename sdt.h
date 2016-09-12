#pragma once
#include <Adafruit_NeoPixel.h>

bool sdtAlarmOrNot;
bool sdtContinualOrNot;
int sdtDuration = 0;

//0为坐，1为非坐
bool sdtState;

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
#define SDT_TIME_THR 7200000
bool s_alarm;//1表示报警

int sdt_count;//用于控制执行频率

//久坐检测部分LED灯的设定
#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT  1
Adafruit_NeoPixel sdtStrip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

void sdt_init()
{
  sdtStrip.begin();
  sdtStrip.show(); // Initialize all pixels to 'off'
  sdtShortP = 0;
  sdtLongP = 0;
  sdtShortAvg = 0;
  sdtShortVar = 0;
  sdtState = 0;
  sdtCount1 = 0;
  sdtCount0 = SDT_LONG_LEN0;
  sdtStartTime = millis();
  sdtAlarmOrNot = 1;
  sdtContinualOrNot = 0;
}

void sdt_judge()
{
  if (!sdtState && (millis() - sdtStartTime) > SDT_TIME_THR)
  {
    //setAlarm();
  }
  sdtShort[sdtShortP] = abs(aaWorld.z) + abs(aaWorld.x) + abs(aaWorld.y);
  //Serial.print(sdtShortP);
  //Serial.print(":");
  //Serial.println(sdtShort[sdtShortP]);
  sdtShortAvg += sdtShort[sdtShortP];
  if (sdtShortP == SDT_SHORT_LEN - 1)
  {
    sdtShortAvg /= (double)SDT_SHORT_LEN;
    Serial.print("  average:");
    Serial.print(sdtShortAvg);
    for (int i = 0; i < SDT_SHORT_LEN; i++)
    {
      sdtShortVar += (sdtShort[i] - sdtShortAvg) * (sdtShort[i] - sdtShortAvg);
    }
    sdtShortVar /= (double)SDT_SHORT_LEN;
    Serial.print("  sdtShortVar:");
    Serial.print(sdtShortVar);
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
      //Serial.print("state:");
      //Serial.println(sdtState);
      sdtStartTime = millis();
      Serial.print("sdtStartTime=");
      Serial.print(sdtStartTime);
    }
    //根据坐的时间，计算0-1的阈值，坐的时间越长，则阈值越大，即越不容易退出坐的状态
    if (!sdtState)
    {
      SDT_LONG_LEN1 = SDT_LONG_LEN1_MIN + (millis() - sdtStartTime) * (SDT_LONG_LEN - SDT_LONG_LEN1_MIN) / SDT_TIME_THR; //最小值为8，最大值为SDT_LONG_LEN
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
    }
    sdtLongP = (sdtLongP + 1) % SDT_LONG_LEN;
    //      Serial.print("  ");
    //      Serial.print(sdtLong[sdtLongP-1]);
    //      Serial.print("  count0=");
    //      Serial.print(sdtCount0);
    //      Serial.print("  count1=");
    //      Serial.print(sdtCount1);
    //      Serial.print("  len=");
    //      Serial.print(SDT_LONG_LEN1);
    //      Serial.print("  state:");
    //      Serial.println(sdtState);
    //LED灯部分
    if (sdtState)
    {
      sdtStrip.setPixelColor(0, sdtStrip.Color(0, 0, 0));
      sdtStrip.show();
    }
    else
    {
      sdtStrip.setPixelColor(0, sdtStrip.Color(25, 0, 0));
      sdtStrip.show();
    }
  }
  sdtShortP = (sdtShortP + 1) % SDT_SHORT_LEN;
}
