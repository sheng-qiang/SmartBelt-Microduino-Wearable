#pragma once
#include "string.h"

int strToInt(String s)
{
  int i = atoi(s.c_str());
  return i;
}

long strToLong(String s)
{
  long i = atol(s.c_str());
  return i;
}

String intToStr(int i)
{
  char s[12];             //设定12位对于存储32位int值足够
  itoa(i, s, 10);            //itoa函数亦可以实现，但是属于C中函数，在C++中推荐用流的方法
  String string_temp = s;
  return string_temp;
}

