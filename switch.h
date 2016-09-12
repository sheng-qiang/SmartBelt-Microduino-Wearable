#pragma once
#include "sdt.h"

//---------------总开关灯----------------------
#define PIXEL_SWITCH    4    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT  1

Adafruit_NeoPixel switchLED = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_SWITCH, NEO_GRB + NEO_KHZ800);

// these constants won't change:
// the piezo is connected to analog pin 0
#define button A0


// these variables will change:
bool buttonNow;      // variable to store the value read from the sensor pin
bool buttonLast;
bool switchLedState;         // variable used to store the last LED status, to toggle the light
int timer;
int switchBeginTime;
//----------------------------------------------

bool switch_init()
{
  switchLED.show(); // Initialize all pixels to 'off'
  buttonNow = 1;      // variable to store the value read from the sensor pin
  buttonLast = 1;
  switchLedState = HIGH;         // variable used to store the last LED status, to toggle the light
  timer = 0;
  switchBeginTime = 0;
}

void switch_judge()
{
  buttonLast = buttonNow;
  buttonNow = analogRead(button);
  // if the sensor reading is greater than the threshold:
  Serial.println(buttonNow);
  if (buttonLast && !buttonNow)
  {
    switchBeginTime = millis();
  }
  if (!buttonNow && switchBeginTime)
  {
    timer = millis() - switchBeginTime;
    if (timer > 3000)
    {
      switchLedState = !switchLedState;
      switchBeginTime = 0;
    }
  }
  if (buttonNow)
  {
    switchBeginTime = 0;
  }
}

