#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <Arduino.h>         
#include <UniversalTelegramBot.h>

enum WFCT{
    DONE,
    WAITING_ON,
    WAITING_OFF
};
extern WFCT waiting_for_ctime;

void handleMessage(UniversalTelegramBot &bot, int index);
void logicHandling(UniversalTelegramBot &bot, const String &text, const String &msg_id);
void saveMode();

#endif