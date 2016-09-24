#pragma once
#include <SPI.h>
#include <SD.h>
#include "BLEhandler.h"
#include "step.h"
#include"swtich.h"
#include "time.h"
#include "sdt.h"
#include "trans.h"
#include "alarm.h"
#include <stdio.h>
//change the serial!
#define mySerial Serial

void notify_handler()
{
  if (mySerial.available() > 0)  //如果串口有数据输入
  {
    switchLED.setPixelColor(1, switchLED.Color(5, 5, 0));
    bool alarmState;
    char c[] = "ALARM_0.TXT";
    //decode info
    String msgTemp = "";
    char c127 = 127;
    msgTemp = mySerial.readStringUntil(c127); //读取到换行符
    int ConPos = msgTemp.indexOf("Connected\r\n");
    if (ConPos != -1)
    {
      msgTemp.remove(ConPos, ConPos + 10);
    }
    //    Serial.println(msgTemp);
    int d[10]; //decoded int array
    int arrTemp[10] = { 0 }; //int array to encode
    bleHandler.decode(msgTemp, d);
    switch (d[0])
    {
      case 0://TIME SYNC
        time_rtc_init(d[2], d[3], d[4], d[1], d[5], d[6], d[7]);//(int _year, int _month, int _day, int _week, int _hour, int _min, int _sec)
        {
          //for test!!
          //        myFile = SD.open("TIMESYNC.TXT", FILE_WRITE);
          //        myFile.println(d[2]);
          //        myFile.println(d[3]);
          //        myFile.println(d[4]); myFile.println(d[5]); myFile.println(d[6]); myFile.println(d[7]); myFile.println(d[1]);
          //TIME SYNC SUCCESS
          arrTemp[0] = 1;
          arrTemp[1] = 1;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        case 2://STEPS SYNC
          //STEPS SYNC HEADER
          myFile = SD.open("STEP_NOI.TXT", FILE_WRITE);
          arrTemp[0] = 3;
          arrTemp[1] = strToInt(myFile.readStringUntil('\n'));
          mySerial.println(bleHandler.encode(arrTemp));
          myFile.close();
          //STEPS SYNC INFO
          myFile = SD.open("STEP_INF.TXT", FILE_WRITE);
          for (int i = 0; i < arrTemp[1]; i++)
          {
            mySerial.println(myFile.readStringUntil('\n'));
          }
          myFile.close();
          arrTemp[0] = 5; //STEP SYNC STOPPER
          arrTemp[1] = 1;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        case 6://STEPS SYNC SUCCESS
          if (d[1])
          {
            SD.remove("STEP_INF.TXT");
            SD.remove("STEP_NOI.TXT");
          }
          break;
        case 7://STEPS SETTINGS
          stepTarget = d[2];
          stepAlarmOrNot = (bool)d[1];

          SD.remove("STEP_SET.TXT");
          myFile = SD.open("STEP_SET.TXT", FILE_WRITE);
          myFile.println(d[1]);
          myFile.println(d[2]);
          myFile.close();

          //STEPS SETTINGS SUCCESS
          arrTemp[0] = 8;
          arrTemp[1] = 1;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        case 9://SENDENTARY SYNC
          //STEPS SYNC HEADER
          myFile = SD.open("SDT_NOI.TXT", FILE_WRITE);
          arrTemp[0] = 10;
          arrTemp[1] = strToInt(myFile.readStringUntil('\n'));
          mySerial.println(bleHandler.encode(arrTemp));
          myFile.close();
          //STEPS SYNC INFO
          myFile = SD.open("SDT_INF.TXT", FILE_WRITE);
          for (int i = 0; i < arrTemp[1]; i++)
          {
            mySerial.println(myFile.readStringUntil('\n'));
          }
          myFile.close();
          arrTemp[0] = 12; //TIME SYNC STOPPER
          arrTemp[1] = 1;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        case 13://SENDENTARY SYNC SUCCESS
          if (d[1])
          {
            SD.remove("SDT_INF.TXT");
            SD.remove("SDT_NOI.TXT");
          }
          break;
        case 14:
          sdtAlarmOrNot = d[1];
          sdtContinualOrNot = d[2];
          sdtDuration = d[3];

          SD.remove("SDT_SET.TXT");
          myFile = SD.open("SDT_SET.TXT", FILE_WRITE);
          myFile.println(d[1]);
          myFile.println(d[2]);
          myFile.println(d[3]);
          myFile.close();

          //STEPS SETTINGS SUCCESS
          arrTemp[0] = 15;
          arrTemp[1] = 1;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        case 16:
          //ALARM UPDATE
          c[6] = d[1] + 48;
          myFile = SD.open(c, FILE_WRITE);
          myFile.println(1);    //switch, default  is on
          myFile.println(d[5]); //mode
          myFile.println(d[2]); //repeatOrNot
          myFile.println(d[3]); //hour
          myFile.println(d[4]); //minute
          myFile.close();
          //ALARM SETTINGS SECCESS
          arrTemp[0] = 18;
          arrTemp[1] = d[1];
          arrTemp[2] = 0;
          arrTemp[3] = 1;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        case 17:
          c[6] = d[1] + 48;
          myFile = SD.open(c);
          alarmState = strToInt(myFile.readStringUntil('\n'));
          myFile.close();
          myFile = SD.open(c, FILE_WRITE);
          myFile.println(!alarmState);   //change the state
          myFile.close();
          //ALARM SETTINGS SECCESS
          arrTemp[0] = 18;
          arrTemp[1] = d[1];
          arrTemp[2] = 1;
          arrTemp[3] = 1;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        default:
          //ERROR:INVALID COMMAND CODE
          arrTemp[0] = 19;
          arrTemp[1] = 0;
          mySerial.println(bleHandler.encode(arrTemp));
          break;
        }
    }
  }
