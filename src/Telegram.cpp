#include <config.h>
#include <Telegram.h>
#include <EEPROM.h>
#include <main.h>
#include <Weather_API.h>


// Everything which is needed for the telegram bot is in here

//They keyboard which shows all the default option
static const char* keyboard =
"["
"  [\"Weather Mode\", \"Full Brightness\"],"
"  [\"Status\"], [\"OFF\"]"
"]";

WFCT waiting_for_ctime = DONE;

void saveMode(uint8_t mode){

  //For saving the current mode in EEPROM memory, to continiue the same after a boot up
  EEPROM.write(0, mode);
  EEPROM.commit();
}

void handleMessage(UniversalTelegramBot &bot, int index){
  
  //Core logics for handling the messages recived on Telegram
  //This function Pre Proccesses the text for handling
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

  //Part of the previous function. This one actually checks the message and act according to it..
  //BLOCKS OF IF ELSE STATEMENTS AHEAD
  if (text == "/start"){
    bot.sendMessageWithReplyKeyboard(msg_id, "Pick an Option:", "", keyboard, false);
    return;
  }

  if(text == "off"){
    digitalWrite(LED_TEST, LOW);
    currentMode = off;
    bot.sendMessage(msg_id, "Turned OFF ❌");
    saveMode(0);
  }

  else if(text == "hi" || text == "hello"){
    bot.sendMessage(msg_id, "HEYY!!!! 😍");
  }

  else if(text == "weather mode" || text == "weathermode"){
   currentMode = weatherOption;
    bot.sendMessage(msg_id, "☁️ Weather Mode Activated!");
    saveMode(1);
  }

  else if(text == "full brightness" || text == "fullbrightness"){
    currentMode = fullBri;
    bot.sendMessage(msg_id, "⚡ Fulllyyyy On mwone! ");
    saveMode(2);
  }

  else if (isNumber(text)){

    int num = 0;
    for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];
    if (c < '0' || c > '9') break;
    num = num * 10 + (c - '0');
    }

    if (num < 0 || num > 100) bot.sendMessage(msg_id, "Between 0 and 100 brooo");
    
    else{
      currentMode = CustomBri;
      currentBri = num;

      setBrightness(num);
      bot.sendMessage(msg_id, "💡 Brightness set to " + text);
      saveMode(3);

    }
  }

  else if(text == "/customtime" || text == "customtime" || text == "/ct"){
    bot.sendMessage(msg_id, "⏰ Send OFF Time (eg: 20:50):");
    waiting_for_ctime = WAITING_OFF;
    return;
  }

  if (waiting_for_ctime == WAITING_OFF){
    CusTimeOn = CTseparator(text, ":");
    waiting_for_ctime = WAITING_ON;
    bot.sendMessage(msg_id, "⏰ Send ON Time:");
    return;
}
  if (waiting_for_ctime == WAITING_ON){
    CusTimeOff = CTseparator(text, ":");
    waiting_for_ctime = DONE;
    currentMode = CustomTime;
    bot.sendMessage(msg_id, "🔥 All Set!!");
    return;
}
  
  else if(text == "/help" || text == "help"){
    bot.sendMessage(msg_id, "\n◆ Do /start to activate the Keyboard ⌨️\n"
    "◆ Send numbers (0-100) to set custom Brightness! 🔅\n"
    "◆ /status to get Current Details 📑"
  );

  }

  else if(text == "/status" || text == "status"){
    
    String mode;
    switch (currentMode){
      case 0:
        mode = "Off";
        break;
      case 1:
        mode = "Weather Mode";
        break;
      case 2:
        mode = "Full Brightness";
        break;
      case 3:
        mode = "Custom Brightness";
        break;

    }
    
    String msg;

    msg  = "*Current Status*\n";
    msg += "➤ 🔅 _Brightness Level_: ";
    msg += String((currentBri * 100) / 1023);
    msg += "%\n";
    msg += "➤ 🌈 _Current Mode_: ";
    msg += mode;
    msg += "\n";
    msg += "➤ ⚠️ _Sun Radiation Level_: ";
    msg += String(currentRad);
    msg += "\n";
    msg += "➤ 🌅 _Sunrise Time_: ";
    msg += mts_to_time(sunriseTime);
    msg += "\n";
    msg += "➤ 🌇 _Sunset Time_: ";
    msg += mts_to_time(sunsetTime);
    msg += "\n";
    msg += "➤ ⌚ _Current Time_: ";
    msg += mts_to_time(currentTime());
    msg += "\n";

    bot.sendMessage(msg_id, msg, "Markdown");
  
  }
  if (text == "cancel" || text == "back"){
    waiting_for_ctime = DONE;
    return;
  }
}