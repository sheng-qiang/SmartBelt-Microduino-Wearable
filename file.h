#pragma once
#include "SD.h"
File myFile;

bool file_init()
{
  bool a=SD.begin(7);
  return a;
}
