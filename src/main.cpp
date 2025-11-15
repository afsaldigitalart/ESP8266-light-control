#include <Arduino.h>
#include <config.h>
#include <WifiModule.h>
#include <Weather_API.h>
#include <Telegram.h>
#include <EEPROM.h>
#include <string>
#include <cctype>
#include <main.h>

//This is super complex and I dont know that is going on anymore

//Global variables (its a mess but it works)
unsigned long lastcallSun = 0;
unsigned long lastcallBOT = 0;
int lastUpdateId = 0;
float currentRad;

SunData data;
MODE currentMode = off;
int CusTimeOn = 0;
int CusTimeOff = 0;

int sunriseTime, sunsetTime;
int srstart = -1, srend = -1, ssstart = -1, ssend = -1;
const char* STATE = "DAY";
int lastss = -1, lastsr = -1;

int currentBri = 0;
int targetBri = 0;



//HELPER FUNCTIONS


int separator(String time, String sep){
  int pos = time.indexOf(sep);
  String Ttime = time.substring(pos+1);
  int hh = Ttime.substring(0,2).toInt();
  int mm = Ttime.substring(3,5).toInt();
  return hh*60+mm;
}

int CTseparator(String time, String sep){
  int pos = time.indexOf(sep);
  String right = time.substring(pos+1);
  int hh = time.substring(0,pos).toInt();
  int mm = right.toInt();
  return hh*60+mm;
}

String mts_to_time(int mts){
  //Converts the give minutes into Hour and Minutes Format
  int time_hr = mts/60;
  int time_mts = mts%60;

  String time_now = String(time_hr) + ":" + String(time_mts);
  return time_now; // returns in hh:mm format
}

float lerp(){

  // Linear Interpolation. For smooth transition between different brightness

  float targetRad;
  float progress;

  if (strcmp(STATE, "SUNRISE")==0){
    progress = (currentTime() - srstart)/25;
    progress = constrain(progress, 0, 1);

    targetRad = progress * data.radiation;
  }

  else if (strcmp(STATE, "SUNSET")==0){
    progress = (currentTime() - ssstart)/30;
    progress = constrain(progress, 0, 1);

    targetRad = (1.0-progress) * data.radiation;
  }

  else targetRad = currentRad;

  currentRad += 0.1 * (targetRad - currentRad);
  return currentRad; //Return value: Current Radiation (the brightness level actually)
}

void smoothUpdate(){

  // LERP for global light transition
  currentBri = currentBri + 0.5 * (targetBri - currentBri);
  analogWrite(LED_TEST, currentBri);
}

bool isNumber(const String &s) {

  /*Checks the given message is Number or Not
  Used in checking Telegram message for Brightness control */
  if (s.length() == 0) return false;

  size_t i = 0;
  if (i == s.length()) return false;

  for (; i < s.length(); i++) {
      if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
          return false;
      }
  }
  return true; //Return Value: True if number else False
}


void oldCheck(){

  //Changes the global variable to avoid two modes working together (total mess)

  /*
  0 - Off
  1 - Weather Mode
  2 - Full Brighness
  3 - Custom Brightness
  */

  uint8_t mode = EEPROM.read(0);

  switch(mode){
    case 0: currentMode = off; break;
    case 1: currentMode = weatherOption;  break;
    case 2: currentMode = fullBri; break;
    case 3: currentMode = CustomBri; break;
  }
}

void setBrightness(int lvl){

  //For setting Custom Brightness according to the input
  currentMode = CustomBri;

  int brighness = map(lvl, 0, 100, 0, 1023);
  brighness = constrain(brighness, 0, 1023);
  targetBri = brighness;

  EEPROM.write(1, lvl);
  EEPROM.commit();
}


void weatherMode(){
  
  //Main Logic for Weather mode. Calls API and changes the brighness according to it
  if (currentTime() >= data.sunrise && currentTime() < data.sunrise+25
  &&lastsr != todayDay() && strcmp(STATE,"SUNRISE") != 0){
    STATE   = "SUNRISE";
    srstart = currentTime();
    srend   = srstart + 25;
    lastsr  = todayDay();
  }

  if ( currentTime() >= data.sunset - 25  && currentTime() < data.sunset + 2 
  && lastss != todayDay()&& strcmp(STATE,"SUNSET") != 0){
    STATE   = "SUNSET";
    ssstart = currentTime();
    ssend   = ssstart + 30;
    lastss  = todayDay();  
  }
  
  if (strcmp(STATE, "SUNRISE") == 0) {
    
    analogWrite(LED_TEST, lerp());
    if(currentTime() >= srend){
        STATE = "DAY";
        currentRad = data.radiation;
    }
    
  }

  else if (strcmp(STATE, "SUNSET")==0){
    
    analogWrite(LED_TEST, lerp());

    if(currentTime() > ssend){
      STATE = "NIGHT";
      currentRad = 0;
    }
  }
  
  else if (strcmp(STATE, "DAY") == 0){
    float targetRad = data.radiation;
    float difference = abs(targetRad - currentRad);

    if (difference > 100) {
      currentRad = targetRad; // Snap to target for large jumps
    } else {
      currentRad += 0.15 * (targetRad - currentRad); // Smooth for small changes
    }
    
    analogWrite(LED_TEST, constrain(currentRad, 0, 1023));
  }

  else if (strcmp(STATE, "NIGHT") == 0){
    currentRad -= 0.08 * currentRad;
    analogWrite(LED_TEST, constrain(currentRad, 0, 1023));
  }

  if (strcmp(STATE,"DAY")==0 && currentTime()>data.sunset) {
    STATE = "NIGHT";
  }
  currentBri = currentRad;
}

void FullBrightness(){
  //For settng the brightness to the maximuum level
  digitalWrite(LED_TEST, HIGH);
  currentBri = 1023;
}

void customTime(){
  if (currentTime() >= CusTimeOn){
    oldCheck();
    if (currentMode == off) currentMode= fullBri;
  }

  else if (currentTime() >= CusTimeOff){
    digitalWrite(LED_TEST, LOW);
  }
}


void setup() {

  Serial.begin(115200);
  EEPROM.begin(4); 

  pinMode(LED_BUILTIN_AUX, OUTPUT);
  pinMode(LED_TEST, OUTPUT);

  setupWifi();

  client.setInsecure();

  while (!isWifiConnected()){
    Serial.println("Connecting...");
    delay(500);
  }

  configTime(19800, 0, "pool.ntps.org");
  time_t now = time(nullptr);

  while (now < 100000) {
    delay(200);
    now = time(nullptr);
  }

  SunData initdata = apiCall();
  data = initdata;
  currentRad = initdata.radiation;
  sunriseTime = data.sunrise;
  sunsetTime = data.sunset;


  oldCheck();

  if (currentMode == CustomBri){
    uint8_t bri = EEPROM.read(1);
    targetBri = map(bri, 0, 100, 0, 1023);
    currentBri = targetBri;
  }
  
}

void loop() {

  
  if (!isWifiConnected()) digitalWrite(LED_BUILTIN_AUX, LOW);
  else digitalWrite(LED_BUILTIN_AUX , HIGH); 

  
  //Telegram Bot Polling Limit
  if (millis() - lastcallBOT >= intervalBOT){
    int numNewMessages = bot.getUpdates(lastUpdateId+1);
    while(numNewMessages){

      for(int i = 0; i < numNewMessages; i++){
        handleMessage(bot, i);
      }

      lastUpdateId = bot.messages[numNewMessages - 1].update_id;
      numNewMessages = bot.getUpdates(lastUpdateId+1);
    }
    lastcallBOT = millis();
  }

  //API Call Limit
  if (millis() - lastcallSun >= intervalSun){
    data = apiCall();

    lastcallSun = millis();
    Serial.print("Current Brigtness : ");
    Serial.println(currentRad);
    sunriseTime = data.sunrise;
    sunsetTime = data.sunset;
  }

  if (currentMode == weatherOption) weatherMode();
  
  else if(currentMode == fullBri) FullBrightness();

  else if(currentMode == CustomBri) smoothUpdate();

  else if(currentMode == CustomTime) customTime();

  else analogWrite(LED_TEST, 0);

  Serial.print("STATE: "); Serial.println(STATE);
  Serial.print("srend: "); Serial.println(srend);
  Serial.print("ctime: "); Serial.println(currentTime());
  Serial.print("radiation: "); Serial.println(data.radiation);


  delay(20);

}

