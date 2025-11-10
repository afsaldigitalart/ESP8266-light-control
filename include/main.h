#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>         
#include <UniversalTelegramBot.h>

extern bool weatherOption;
extern bool fullBri;
extern bool customBri;
extern int currentBri;
extern float currentRad;
extern String sunriseTime;
extern String sunsetTime;

float lerp();
void weatherMode();
void FullBrightness();
void oldCheck();
void setBrightness(int lvl);
bool isNumber(const String &s);

#endif