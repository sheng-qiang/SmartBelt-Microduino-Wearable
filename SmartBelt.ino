#include "Wire.h"
#include "I2Cdev.h"
#include "mpu.h"
#include <SoftwareSerial.h>
#include "BLEhandler.h"
#include "notificationHandler.h"
#include"switch.h"
#include "file.h"
#include <avr/wdt.h>
//设定看门狗时间为1s
#define TIMEOUT WDTO_1S

void setup() {
  Wire.begin();
  Serial.begin(115200);
  mySerial.begin(9600);
  while (!Serial);

  switch_init();
  mpu_init();
  step_init();
  sdt_init();
  SD_init();

  wdt_enable(TIMEOUT);
  Serial.println("WDT reset");
}

void loop() 
{
  wdt_reset();
  switch_judge();

  //LED FOR TEST
  if (switchLedState)
  {
    switchLED.setPixelColor(0, switchLED.Color(0, 0, 0));
    switchLED.show();
  }
  else
  {
    switchLED.setPixelColor(0, switchLED.Color(0, 25, 0));
    switchLED.show();

    if (!dmpReady) return;
    fifoCount = mpu.getFIFOCount();
    if (fifoCount == 1024)
    {
      mpu.resetFIFO();
      Serial.println(F("FIFO overflow!"));
    }
    else
    {
      mpu_get();
      step_detect();
      if (sdt_count == 0) sdt_judge();
      sdt_count = (sdt_count + 1) % 50;
    }
  }
  notify_handler();
}
