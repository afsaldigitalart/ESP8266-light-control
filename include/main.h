#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>         
#include <UniversalTelegramBot.h>

extern bool weatherOption;
extern bool fullBri;
extern bool customBri;
extern int currentBri;
extern float currentRad;
extern int sunriseTime;
extern int sunsetTime;

float lerp();
void weatherMode();
void FullBrightness();
void oldCheck();
void setBrightness(int lvl);
bool isNumber(const String &s);
String mts_to_time(int mts);

#endif