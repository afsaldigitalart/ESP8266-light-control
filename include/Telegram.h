#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <Arduino.h>         
#include <UniversalTelegramBot.h>

void handleMessage(UniversalTelegramBot &bot, int index);
void logicHandling(UniversalTelegramBot &bot, const String &text, const String &msg_id);

#endif