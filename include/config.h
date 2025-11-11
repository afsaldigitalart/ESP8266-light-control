#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <UniversalTelegramBot.h>

#define LED_TEST D5

extern WiFiClientSecure client;
extern HTTPClient http;

extern const char* BOT_API;
extern UniversalTelegramBot bot;

extern const char* WAPI;
extern const char* SSID;
extern const char* PASS;

extern const char* allowedChatId[];

extern int allowedCount;
extern const unsigned long intervalSun;
extern const unsigned long intervalBOT;
const float alpha;

#endif