#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>         
#include <UniversalTelegramBot.h>

enum MODE{
  off,
  weatherOption,
  fullBri,
  CustomBri,
  CustomTime,
};
extern MODE currentMode;

extern float currentBri;
extern float currentRad;
extern int sunriseTime;
extern int sunsetTime;
extern int CusTimeOn;
extern int CusTimeOff;
extern String mode;
extern int RunTime;

float lerp();
bool inWindow(int now, int start, int duration);
void weatherMode();
void FullBrightness();
void oldCheck();
void setBrightness(int lvl);
bool isNumber(const String &s);
int separator(String time, String sep);
int CTseparator(String time, String sep);
String mts_to_time(int mts);
void customTime();

#endif