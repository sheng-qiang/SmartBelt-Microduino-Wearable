#pragma once
#define buzzer_pin 4
#define ALARM_NUM 6

struct AlarmParam
{
  bool openOrNot;
  byte h;
  byte m;
  bool mode[7];
  bool repeatOrNot;
  bool repeatAllow;
  bool repeatOnce;
  bool repeatTwice;
} alarms[ALARM_NUM];

bool sdtAlarmFlag = 1;

struct modeSettings
{
  int times;
  int highPart;
  int lowPart;
  float level;
};

const struct modeSettings modes[3] = {{3, 1500, 500, 500}, {5, 500, 250, 400}, {4, 1000, 500, 250}};

const int toneMode[8] = {1, 1, 1, 1, 1, 1, 0, 2};

class AlarmHandler
{
  public:
    AlarmHandler()
    {
      toneNow = -1;
      buzzerSpeak = false;
    }
    void start_alarm(int toneNo);
    void to_alarm();
    int get_tone_now();
    void stop_tone_now();
    void reset_tone_now();
  private:
    int toneNow;
    int countTone;
    unsigned long startTime;
    bool buzzerSpeak;
    bool stopFlag;
};

int AlarmHandler::get_tone_now()
{
  return toneNow;
}

void AlarmHandler::reset_tone_now()
{
  toneNow = -1;
  Serial.println("toneNow reset.");
}

void AlarmHandler::stop_tone_now()
{
  noTone(buzzer_pin);
  //  stopFlag = 1;
  if (toneNow == -1) 
  {
    switchStateMode2=0;
    return;
  }
  if (toneNow >= 0 && toneNow <= 2)
  {
    //    Serial.print(alarms[toneNow].repeatAllow);
    //    Serial.println("NOT ALLOW");
    alarms[toneNow].repeatAllow = 0;
  }
  if (toneNow == 7)
  {
    sdtAlarmFlag = 0;
  }
  toneNow = -1;
  switchStateMode2=0;
  return;
}

void AlarmHandler::start_alarm(int toneNo)
{
  noTone(buzzer_pin);
  toneNow =  toneNo;
  countTone = 0;
  startTime = millis();
  buzzerSpeak = 0;
  stopFlag = 0;
}

void AlarmHandler::to_alarm()
{
  if (toneNow != -1 && !stopFlag)
  {
    int diffTime = millis() - startTime;
    int duration = modes[toneMode[toneNow]].highPart + modes[toneMode[toneNow]].lowPart;
    countTone = diffTime / duration;
    if (diffTime >= duration * countTone  && diffTime <= modes[toneMode[toneNow]].highPart + countTone * duration)
    {
      if (countTone < modes[toneMode[toneNow]].times)
      {
        tone(buzzer_pin, modes[toneMode[toneNow]].level);
      }
      else
      {
        noTone(buzzer_pin);
        stopFlag = 1;
//        Serial.println("this tone has STOPPED");
      }
    }
    else
    {
      noTone(buzzer_pin);
    }
  }
}

AlarmHandler alarmHandler;
