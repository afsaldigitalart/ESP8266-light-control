#include <config.h>
#include <credentials.h>

WiFiClientSecure client;
HTTPClient http;
UniversalTelegramBot bot(BOT_API, client);

const char* allowedChatId[] = {"1856668154", "5307041271"};
int allowedCount = sizeof(allowedChatId)/sizeof(allowedChatId[0]);

const unsigned long intervalSun = 600000;
const unsigned long intervalBOT = 500;