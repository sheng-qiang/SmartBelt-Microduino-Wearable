#include "time.h"
#define buzzer_pin 6


struct modeSettings
{
  int times;
  int highPart;
  int lowPart;
  float level;
};

const struct modeSettings modes[3] = {{3, 1500, 500, 500}, {5, 1000, 500, 1000}, {5, 1000, 500, 250}};

class AlarmHandler
{
  public:
    void start_alarm(int mode);
    void to_alarm();
    void stop_alarm();

  private:
    int toneNow;
    int countTone;
    int startTime;
    bool firstTime;
    bool buzzerSpeak;
};

void AlarmHandler::start_alarm(int mode)
{
  toneNow =  mode;
  countTone = 0;
  startTime = millis();
  firstTime = true;
  buzzerSpeak = true;
}

void AlarmHandler::to_alarm()
{
  if (buzzerSpeak)
  {
    int diffTime = millis() - startTime;
    int duration = modes[toneNow].highPart + modes[toneNow].lowPart;
    if (diffTime >= duration * (countTone  && diffTime <= modes[toneNow].highPart + countTone * duration))
    {
      if (firstTime)
      {
        tone(buzzer_pin, modes[toneNow].level);
        firstTime = false;
      }
    }
    else
    {
      noTone(buzzer_pin);
      countTone++;
      if (countTone >= modes[toneNow].times)
      {
        buzzerSpeak = false;
      }
    }
  }
  else
  {
    noTone(buzzer_pin);
  }
}

void AlarmHandler::stop_alarm()
{
  buzzerSpeak = false;
}

AlarmHandler alarmHandler;
