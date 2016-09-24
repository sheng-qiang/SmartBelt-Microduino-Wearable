#pragma once
#include "trans.h"
#define BLE_CMD_NUM_MAX 19

const char encodeTable[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
const int numOfParam[BLE_CMD_NUM_MAX + 1] = { 7, 1, 0, 1, 8, 0, 1, 2, 1, 0, 1, 7, 0, 1, 3, 1, 5, 1, 3, 1 };
const int numOfBytes[BLE_CMD_NUM_MAX + 1] = { 7, 1, 1, 3, 9, 1, 1, 2, 1, 1, 3, 7, 1, 1, 2, 1, 5, 2, 2, 1 };
struct cutInfo
{
  int high;
  int i;
  int low;
};

cutInfo cutTable[BLE_CMD_NUM_MAX + 1][8] = {
  { { 5, 1, 3 },  { 2, 1, 0 }, { 7, 2, 0 }, { 7, 3, 0 }, { 7, 4, 0 }, { 7, 5, 0 }, { 7, 6, 0 }, { 7, 7, 0 } },
  { { 1, 0, 1 } },
  { { 0, 0, 0 } },
  { { 14, 1, 0 } },
  { { 6, 1, 4 }, { 3, 1, 0 }, { 7, 2, 0 }, { 7, 3, 0 }, { 7, 4, 0 }, { 7, 5, 0 }, { 7, 6, 0 }, { 14, 7, 0 } },
  { { 0, 0, 0 } },
  { { 1, 0, 1 } },
  { { 1, 0, 1 }, { 6, 1, 0 } },
  { { 1, 0, 1 } },
  { { 0, 0, 0 } },
  { { 14, 1, 0 } },
  { { 7, 1, 4 }, { 3, 1, 0 }, { 7, 2, 0 }, { 7, 3, 0 }, { 7, 4, 0 }, { 7, 5, 0 }, { 7, 6, 0 } },
  { { 0, 0, 0 } },
  { { 1, 0, 1 } },
  { { 4, 1, 4 }, { 3, 1, 3 }, { 2, 1, 0 } },
  { { 1, 0, 1 } },
  { { 3, 1, 1 }, { 0, 1, 0 }, { 7, 2, 0 }, { 7, 3, 0 }, { 6, 4, 0 } },
  { { 2, 1, 0 } },
  { { 5, 1, 3 }, { 2, 1, 1 }, { 0, 1, 0 } },
  { { 1, 0, 0 } }
};

class BLEhandler
{
  public:
    void decode(String& receivedInfo, unsigned int* decodedInfo);
    void encode(String& encodedInfo, unsigned int* encodeInfo);
  private:
    byte get_part(int high, char c, int low); // high,low ranged 8~1
    byte set_part(int info, int low);
};

void BLEhandler::decode(String& receivedInfo, unsigned int* decodedInfo)
{
  int type = (receivedInfo[0] & 0xFC) >> 2;
  Serial.print("type= ");
  Serial.println(type);
  decodedInfo[0] = type;
  if (numOfParam[type] == 0 || type > BLE_CMD_NUM_MAX)  return;
  else
  {
    cutInfo* p = cutTable[type];
    for (int pos = 0; pos < numOfParam[type]; pos++)
      if ((p + pos)->high != 14)
      {
        decodedInfo[pos + 1] = get_part((p + pos)->high, receivedInfo[(p + pos)->i], (p + pos)->low);
      }
      else
      {
        decodedInfo[pos + 1] = get_part(5, receivedInfo[(p + pos)->i], 0) * 64 + get_part(5, receivedInfo[(p + pos)->i + 1], 0);
      }
  }
}

void BLEhandler::encode(String& encodedInfo, unsigned int* encodeInfo)
{
  byte encodeBytes[10] = {0};
  encodeBytes[0] = set_part(encodeInfo[0], 2);
  if (encodeInfo[0] < 0 || encodeInfo[0] > BLE_CMD_NUM_MAX)
  {
    return;
  }
  Serial.print("encodeBytes[0]=");
  Serial.println(encodeBytes[0]);
  //  Serial.print("encodeInfo[0]=");
  //  Serial.println(encodeInfo[0]);
  cutInfo* p = cutTable[encodeInfo[0]];
  for (int pos = 0; pos < numOfParam[encodeInfo[0]]; pos++)
  {
    if ((p + pos)->high != 14)
    {
      encodeBytes[(p + pos)->i] += set_part(encodeInfo[pos + 1], (p + pos)->low);
    }
    else
    {
      encodeBytes[(p + pos)->i] += set_part(encodeInfo[pos + 1] / 64, 0);
      encodeBytes[(p + pos)->i + 1] += set_part(encodeInfo[pos + 1] % 64, 0);
    }
  }
  for (int pos = 0; pos < numOfBytes[encodeInfo[0]]; pos++)
  {
    char c = encodeBytes[pos];
    if ( c != 0)
    {
      encodedInfo += String(c);
    }
    else
    {
      encodedInfo += String(0);
      encodedInfo[pos] -= 48;
    }
  }
    char endMark =127;
    encodedInfo += String(endMark);
}

byte BLEhandler::get_part(int high, char c, int low)
{
  if (high == 7 && low == 0)
  {
    return (byte)(c);
  }
  byte parter = 0;
  for (int i = high; i >= low; i--)
    parter += encodeTable[i];
  return (byte)((c & parter) >> low);
}

byte BLEhandler::set_part(int info, int low)
{
  return (byte)(info << low);
}

BLEhandler bleHandler;
