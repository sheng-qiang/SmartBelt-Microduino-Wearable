#pragma once
#include "trans.h"
#define BLE_CMD_NUM 20

const char encodeTable[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
const int numOfParam[BLE_CMD_NUM + 1] = { 7, 1, 0, 1, 8, 0, 1, 2, 1, 0, 1, 7, 0, 1, 3, 1, 5, 5, 1, 3, 1 };
const int numOfBytes[BLE_CMD_NUM + 1] = { 6, 1, 1, 3, 9, 1, 1, 2, 1, 1, 3, 7, 1, 1, 2, 1, 4, 4, 1, 2, 1 };
struct cutInfo
{
  int high;
  int i;
  int low;
};

cutInfo cutTable[BLE_CMD_NUM + 1][8] = {
  { { 2, 0, 0 }, { 7, 1, 4 }, { 3, 1, 0 }, { 7, 2, 0 }, { 7, 3, 0 }, { 7, 4, 0 }, { 7, 5, 0 } },
  { { 2, 0, 2 } },
  { { 0, 0, 0 } },
  { { 15, 1, 0 } },
  { { 7, 1, 4 }, { 3, 1, 0 }, { 7, 2, 0 }, { 7, 3, 0 }, { 7, 4, 0 }, { 7, 5, 0 }, { 7, 6, 0 }, { 15, 7, 0 } },
  { { 0, 0, 0 } },
  { { 2, 0, 2 } },
  { { 2, 0, 2 }, { 7, 1, 0 } },
  { { 2, 0, 2 } },
  { { 0, 0, 0 } },
  { { 15, 1, 0 } },
  { { 7, 1, 4 }, { 3, 1, 0 }, { 7, 2, 0 }, { 7, 3, 0 }, { 7, 4, 0 }, { 7, 5, 0 }, { 7, 6, 0 } },
  { { 0, 0, 0 } },
  { { 2, 0, 2 } },
  { { 7, 1, 7 }, { 6, 1, 6 }, { 5, 1, 3 } },
  { { 2, 0, 2 } },
  { { 7, 1, 5 }, { 4, 1, 0 }, { 7, 2, 0 }, { 7, 3, 1 }, { 0, 3, 0 } },
  { { 7, 1, 5 }, { 4, 1, 0 }, { 7, 2, 0 }, { 7, 3, 1 }, { 0, 3, 0 } },
  { { 2, 0, 0 } },
  { { 7, 1, 5 }, { 4, 1, 3 }, { 2, 1, 2 } },
  { { 2, 0, 0 } },
};

class BLEhandler
{
  public:
    void decode(String receivedInfo, int* decodedInfo);
    String encode(int* encodeInfo);
    int decodeInfo[10];
  private:
    byte get_part(int high, char c, int low); // high,low ranged 8~1
    char set_part(int info, int low);
};

void BLEhandler::decode(String receivedInfo, int* decodedInfo)
{
  int type = (int)(receivedInfo[0] & 0xf8) >> 3;
  decodedInfo[0] = type;
  if (numOfParam[type] == 0 || type > 20)  return;
  else
  {
    cutInfo* p = cutTable[type];
    for (int pos = 0; pos < numOfParam[type]; pos++)
      if ((p + pos)->high != 15)
      {
        decodedInfo[pos + 1] = get_part((p + pos)->high, receivedInfo[(p + pos)->i], (p + pos)->low);
      }
      else
      {
        decodedInfo[pos + 1] = get_part(7, receivedInfo[(p + pos)->i], 0) * 256 + get_part(7, receivedInfo[(p + pos)->i + 1], 0);
      }
  }
}

String BLEhandler::encode(int* encodeInfo)
{
  if (encodeInfo[0] < 0 || encodeInfo[0] > BLE_CMD_NUM)
    return "";
  char temp[10] = { 0 };
  temp[0] = set_part(encodeInfo[0], 3);
  cutInfo* p = cutTable[encodeInfo[0]];
  for (int pos = 0; pos < numOfParam[encodeInfo[0]]; pos++)
  {
    if ((p + pos)->high != 15)
    {
      temp[(p + pos)->i] += set_part(encodeInfo[pos + 1], (p + pos)->low);;
    }
    else
    {
      temp[(p + pos)->i] = set_part(encodeInfo[pos + 1] / 256, 0);
      temp[(p + pos)->i + 1] = set_part(encodeInfo[pos + 1] % 256, 0);
    }
  }
  return temp;
}

//String BLEhandler::error(int code)
//{
//  int encodeInfo[2] = { 20, code };
//  String errorInfo = encode(encodeInfo);
//  return errorInfo;
//}


byte BLEhandler::get_part(int high, char c, int low)
{
  if (high == 7 && low == 0)
    return (byte)(c);
  byte parter = 0; //to fetch high~low bits
  for (int i = high; i >= low; i--)
    parter += encodeTable[i];
  return (byte)((c & parter) >> low);
}

char BLEhandler::set_part(int info, int low)
{
  return ((char)info << low);
}

BLEhandler bleHandler;
