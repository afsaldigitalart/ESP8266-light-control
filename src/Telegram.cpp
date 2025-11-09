#include <config.h>
#include <Telegram.h>
#include <EEPROM.h>
#include <main.h>

static const char* keyboard =
"["
"  [\"Weather Mode\", \"Full Brightness\"],"
"  [\"OFF\"]"
"]";


void saveMode(uint8_t mode){
  EEPROM.write(0, mode);
  EEPROM.commit();
}

void handleMessage(UniversalTelegramBot &bot, int index){
  
  bool allowed = false;
  auto &msg = bot.messages[index];
  String msg_id = msg.chat_id;

  String text = msg.text;
  text.toLowerCase();

  for(int i = 0; i<allowedCount; i++){
    if(msg_id == allowedChatId[i]) allowed = true;
  }

  if(!allowed){
    bot.sendMessage(msg_id, "Nigga you ain't allowed to talk with me!", "");
    return;
  }

  bot.sendMessageWithReplyKeyboard(msg_id, "", "", keyboard, true);

  logicHandling(bot, text, msg_id);
}

void logicHandling(UniversalTelegramBot &bot, const String &text, const String &msg_id ){

  if (text == "/start"){
    bot.sendMessageWithReplyKeyboard(msg_id, "Pick an Option:", "", keyboard, false);
    return;
  }

  if(text == "off"){
    digitalWrite(LED_TEST, LOW);
    saveMode(0);
    fullBri = weatherOption = false;
    bot.sendMessage(msg_id, "Turned OFF ❌");
  }

  else if(text == "hi" || text == "hello"){
    bot.sendMessage(msg_id, "HEYY!!!! 😍");
  }

  else if(text == "weather mode" || text == "weathermode"){
    if(fullBri) fullBri = false;
    weatherOption = true;
    saveMode(1);
    bot.sendMessage(msg_id, "☁️ Weather Mode Activated!");
  }

  else if(text == "full brightness" || text == "fullbrightness"){
    if(weatherOption) weatherOption = false;
    fullBri = true;
    saveMode(2);
    bot.sendMessage(msg_id, "⚡ Fulllyyyy On mwone! ");
  }

  else{
    bot.sendMessage(msg_id, "Vere commands onnum allowed alla mwonuuu 😓");
  }
}