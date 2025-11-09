#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>         
#include <UniversalTelegramBot.h>

extern bool weatherOption;
extern bool fullBri;

float lerp();
void weatherMode();
void FullBrightness();
void oldCheck();

#endif