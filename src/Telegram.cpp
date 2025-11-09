#include <config.h>
#include <Telegram.h>

static const String keyboard = "[[\"ON\", \"OFF\"]]";

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

  bot.sendMessageWithReplyKeyboard(msg_id, "", "", keyboard, false);

  logicHandling(bot, text, msg_id);
}

void logicHandling(UniversalTelegramBot &bot, const String &text, const String &msg_id ){

  if (text == "/start"){
    bot.sendMessageWithReplyKeyboard(msg_id, "Pick an Option:", "", keyboard, false);
    return;
  }

  if(text == "on" ){
    digitalWrite(LED_TEST, HIGH);
    bot.sendMessage(msg_id, "Turned ON! 💡");
  }

  else if(text == "off"){
    digitalWrite(LED_TEST, LOW);
    bot.sendMessage(msg_id, "Turned OFF ❌");
  }

  else if(text == "hi" || text == "hello"){
    bot.sendMessage(msg_id, "HEYY!!!! 😍");
  }

  else{
    bot.sendMessage(msg_id, "Vere commands onnum allowed alla mwonuuu 😓");
  }
}