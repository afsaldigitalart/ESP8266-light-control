#include <Arduino.h>
#include <config.h>
#include <WifiModule.h>
#include <Weather_API.h>
#include <Telegram.h>
#include <EEPROM.h>
#include <string>
#include <cctype>

//0 - Off
//1 - Weather Mode
//2 - Full Brighness
//3 - Custom Brightness

unsigned long lastcallSun = 0;
unsigned long lastcallBOT = 0;
int lastUpdateId = 0;
float currentRad;
SunData data;

bool weatherOption = false;
bool fullBri = false;
bool customBri = true;

String sunriseTime, sunsetTime;
int srstart = -1, srend = -1, ssstart = -1, ssend = -1;
const char* STATE = "DAY";
int lastss = -1, lastsr = -1;

float alpha = 0.1;
int currentBri = 0;
int targetBri = 0;

float lerp(){
  float progress = (currentTime() - srstart)/25;
  progress = constrain(progress, 0, 1);

  float targetRad = progress * data.radiation;
  currentRad += 0.1 * (targetRad - currentRad);
  return currentRad;
}

void smoothUpdate(){
    currentBri = currentBri + alpha * (targetBri - currentBri);
    analogWrite(LED_TEST, currentBri);
}

bool isNumber(const String &s) {
    if (s.length() == 0) return false;

    size_t i = 0;
    if (i == s.length()) return false;

    for (; i < s.length(); i++) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
            return false;
        }
    }
    return true;
}

void weatherMode(){
  if (currentTime() >= data.sunrise && lastsr != todayDay() && strcmp(STATE,"SUNRISE") != 0){
    STATE   = "SUNRISE";
    srstart = currentTime();
    srend   = srstart + 25;
    lastsr  = todayDay();
  }

  if ( currentTime() >= data.sunset - 25 && lastss != todayDay() && strcmp(STATE,"SUNSET") != 0){
    STATE   = "SUNSET";
    ssstart = currentTime();
    ssend   = ssstart + 30;
    lastss  = todayDay();  
  }
  
  if (strcmp(STATE, "SUNRISE") == 0) {
    
    analogWrite(LED_TEST, lerp());

    if(currentTime() > srend){
    STATE = "DAY";
    }
  }

  else if (strcmp(STATE, "SUNSET")==0){
    
    analogWrite(LED_TEST, lerp());

    if(currentTime() > ssend){
      STATE = "NIGHT";
    }
  }
  
  else if (strcmp(STATE, "DAY") == 0){
    float targetRad = data.radiation;
    currentRad += 0.08 * (targetRad - currentRad);
    analogWrite(LED_TEST, constrain(currentRad, 0, 1023));
  }

  else if (strcmp(STATE, "NIGHT") == 0){
    currentRad -= 0.08 * currentRad;
    analogWrite(LED_TEST, constrain(currentRad, 0, 1023));
  }

  if (strcmp(STATE,"DAY")==0 && data.radiation < 1 && currentTime()>ssend) {
    STATE = "NIGHT";
  }
}

void FullBrightness(){
  digitalWrite(LED_TEST, HIGH);
}

void oldCheck(){
  uint8_t mode = EEPROM.read(0);

  switch(mode){
    case 0:   fullBri = false; weatherOption = false;customBri = false; break;
    case 1:   weatherOption = true; fullBri = false;customBri = false;  break;
    case 2:   fullBri = true; weatherOption = false;customBri = false; break;
    case 3:   fullBri = false; weatherOption = false; customBri = true; break;
}
}

void setBrightness(int lvl){
  if(weatherOption || fullBri) weatherOption = fullBri = false;
  customBri = true;
  int brighness = map(lvl, 0, 100, 0, 1023);
  brighness = constrain(brighness, 0, 1023);
  targetBri = brighness;
  EEPROM.write(1, lvl);
  EEPROM.commit();
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
  oldCheck();

  if (customBri){
    uint8_t bri = EEPROM.read(1);
    targetBri = map(bri, 0, 100, 0, 1023);
    currentBri = targetBri;
  }
  
}

void loop() {

  if (!isWifiConnected()) digitalWrite(LED_BUILTIN_AUX, LOW);
  else digitalWrite(LED_BUILTIN_AUX, HIGH);

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

  if (millis() - lastcallSun >= intervalSun){
    data = apiCall();
    lastcallSun = millis();
    Serial.print("Current Brigtness : ");
    Serial.println(currentRad);
    sunriseTime = data.sunrise;
    sunsetTime = data.sunset;
  }

  if (weatherOption) weatherMode();
  
  else if(fullBri) FullBrightness();

  else if(customBri) smoothUpdate();

  else analogWrite(LED_TEST, 0);

  delay(20);

}

