#pragma once
#include <SPI.h>
#include <SD.h>
#include "step.h"
#include "time.h"
#include "BLEhandler.h"
#include "sdt.h"
#include "trans.h"
#include "alarm.h"
#include <stdio.h>

#define mySerial Serial1

void SD_init()
{
  if (!SD.begin(7)) {
    Serial.println("SD initialization failed!");
    return;
  }
  else
  {
    Serial.println("SD initialization done.");
  }
}

void notify_handler()
{
  if (mySerial.available() > 0)  //如果串口有数据输入
  {
    bool timeSyncSuccess;
    bool alarmState;
    char c[] = "alarm_0.txt";
    //decode info
    String msgTemp = "";
    msgTemp = mySerial.readStringUntil('\n'); //读取到换行符
    int d[10]; //decoded int array
    int arrTemp[10] = { 0 }; //int array to encode
    bleHandler.decode(msgTemp, d);
    switch (d[0])
    {
      case 0://TIME SYNC
        timeSyncSuccess = time_set(d[2], d[3], d[4], d[5], d[6], d[7], d[1]);
        //TIME SYNC SUCCESS
        arrTemp[0] = 1;
        arrTemp[1] = timeSyncSuccess;
        mySerial.println(bleHandler.encode(arrTemp));
        break;
      case 2://STEPS SYNC
        //STEPS SYNC HEADER
        myFile = SD.open("step_numOfInfo.txt", FILE_WRITE);
        arrTemp[0] = 3;
        arrTemp[1] = strToInt(myFile.readStringUntil('\n'));
        mySerial.println(bleHandler.encode(arrTemp));
        myFile.close();
        //STEPS SYNC INFO
        myFile = SD.open("step.txt", FILE_WRITE);
        for (int i = 0; i < arrTemp[1]; i++)
        {
          mySerial.println(myFile.readStringUntil('\n'));
        }
        myFile.close();
        arrTemp[0] = 5; //TIME SYNC STOPPER
        arrTemp[1] = 1;
        mySerial.println(bleHandler.encode(arrTemp));
        break;
      case 6://STEPS SYNC SUCCESS
        if (d[1])
        {
          SD.remove("step.txt");
          SD.remove("step_numOfInfo.txt");
        }
        break;
      case 7://STEPS SETTINGS
        stepTarget = d[2];
        stepAlarmOrNot = (bool)d[1];

        SD.remove("step_settings.txt");
        myFile = SD.open("step_settings.txt", FILE_WRITE);
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
        myFile = SD.open("sdt_numOfInfo.txt", FILE_WRITE);
        arrTemp[0] = 10;
        arrTemp[1] = strToInt(myFile.readStringUntil('\n'));
        mySerial.println(bleHandler.encode(arrTemp));
        myFile.close();
        //STEPS SYNC INFO
        myFile = SD.open("sdt.txt", FILE_WRITE);
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
          SD.remove("sdt.txt");
          SD.remove("sdt_numOfInfo.txt");
        }
        break;
      case 14:
        sdtAlarmOrNot = d[1];
        sdtContinualOrNot = d[2];
        sdtDuration = d[3];

        SD.remove("sdt_settings.txt");
        myFile = SD.open("sdt_settings.txt", FILE_WRITE);
        myFile.println(d[1]);
        myFile.println(d[2]);
        myFile.println(d[3]);
        myFile.close();

        //STEPS SETTINGS SUCCESS
        arrTemp[0] = 15;
        arrTemp[1] = 1;
        mySerial.println(bleHandler.encode(arrTemp));
        break;
      case 17:
        //ALARM UPDATE
        c[6] = d[2] + 48;
        myFile = SD.open(c, FILE_WRITE);
        myFile.println(1);    //switch, default  is on
        myFile.println(d[4]); //mode
        myFile.println(d[5]); //repeatOrNot
        myFile.println(d[1]); //hour
        myFile.println(d[3]); //minute
        myFile.close();
        //ALARM SETTINGS SECCESS
        arrTemp[0] = 19;
        arrTemp[1] = d[2];
        arrTemp[2] = arrTemp[3] = 1;
        mySerial.println(bleHandler.encode(arrTemp));
        break;
      case 18:
        c[6] = d[1] + 48;
        myFile = SD.open(c);
        alarmState = strToInt(myFile.readStringUntil('\n'));
        myFile.close();
        myFile = SD.open(c, FILE_WRITE);
        myFile.println(!alarmState);   //change the state
        myFile.close();
        //ALARM SETTINGS SECCESS
        arrTemp[0] = 19;
        arrTemp[1] = d[2];
        arrTemp[2] = 2;
        arrTemp[3] = 1;
        mySerial.println(bleHandler.encode(arrTemp));
        break;
      default:
        //ERROR:INVALID COMMAND CODE
        mySerial.println(bleHandler.encode(bleHandler.error(0)));
        break;
    }
  }
}

