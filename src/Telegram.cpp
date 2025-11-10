#include <config.h>
#include <Telegram.h>
#include <EEPROM.h>
#include <main.h>
#include <Weather_API.h>

static const char* keyboard =
"["
"  [\"Weather Mode\", \"Full Brightness\"],"
"  [\"Status\"], [\"OFF\"]"
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
    fullBri = weatherOption = customBri = false;
    bot.sendMessage(msg_id, "Turned OFF ❌");
    saveMode(0);
  }

  else if(text == "hi" || text == "hello"){
    bot.sendMessage(msg_id, "HEYY!!!! 😍");
  }

  else if(text == "weather mode" || text == "weathermode"){
    if(fullBri || customBri) fullBri = customBri = false;
    weatherOption = true;
    bot.sendMessage(msg_id, "☁️ Weather Mode Activated!");
    saveMode(1);
  }

  else if(text == "full brightness" || text == "fullbrightness"){
    if(weatherOption || customBri) weatherOption = customBri = false;
    fullBri = true;
    bot.sendMessage(msg_id, "⚡ Fulllyyyy On mwone! ");
    saveMode(2);
  }

  else if (isNumber(text)){

    int num = 0;
    for (int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c < '0' || c > '9') break;
    num = num * 10 + (c - '0');
    }

    if (num < 0 || num > 100) bot.sendMessage(msg_id, "Between 0 and 100 brooo");
    
    else{
      if(weatherOption || fullBri) weatherOption = fullBri = false;
      customBri = true;

      setBrightness(num);
      bot.sendMessage(msg_id, "Brightness set to " + text);
      saveMode(3);

    }
  }

  else if(text == "/help" || text == "help"){
    bot.sendMessage(msg_id, "Do /start to activate the Keyboard ⌨️\n"
    "Send numbers (0-100) to set custom Brightness! 🔅\n"
    "/status to get Current Details 📑"
  );

  }

  else if(text == "/status" || text == "status"){
    
    String msg;
    int time_hr = currentTime()/60;
    int time_mts = currentTime()%60;

    String time_now = time_hr + ":" + time_mts;

    msg  = "*Current Status*\n";
    msg += "🔅 _Brightness Level_: ";
    msg += String((currentBri * 100) / 1023);
    msg += "%\n";
    msg += "⚠️ _Sun Radiation Level_: ";
    msg += String(currentRad);
    msg += "%\n";
    msg += "🌅 _Sunrise Time (GMT 5:30)_: ";
    msg += sunriseTime;
    msg += "%\n";
    msg += "🌇 _Sunset Time (GMT 5:30)_: ";
    msg += sunsetTime;
    msg += "%\n";
    msg += "⌚ _Current Time_: ";
    msg += time_now;
    msg += "%\n";

    bot.sendMessage(msg_id, msg, "Markdown");
  
  }

  else{
    bot.sendMessage(msg_id, "Vere commands onnum allowed alla mwonuuu 😓");
  }
}