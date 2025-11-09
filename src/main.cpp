#include <Arduino.h>
#include <config.h>
#include <WifiModule.h>
#include <Weather_API.h>
#include <Telegram.h>

unsigned long lastcallSun = 0;
unsigned long lastcallBOT = 0;
int lastUpdateId = 0;
float currentRad;
SunData data;


void setup() {

  Serial.begin(115200); 
  Serial.println("BOOT");

  pinMode(LED_WIFI, OUTPUT);
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
}

int srstart = -1, srend = -1, ssstart = -1, ssend = -1; 
const char* STATE = "DAY";
int lastss = -1, lastsr = -1;

void loop() {

  if (!isWifiConnected()) digitalWrite(LED_WIFI, LOW);
  else digitalWrite(LED_WIFI, HIGH);

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
  }

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
    float progress = (currentTime() - srstart)/25;
    progress = constrain(progress, 0, 1);

    float targetRad = progress * data.radiation;
    currentRad += 0.1 * (targetRad - currentRad);
    analogWrite(LED_TEST, currentRad);

    if(currentTime() > srend){
    STATE = "DAY";
    }
  }

  else if (strcmp(STATE, "SUNSET")==0){
    float progress = (currentTime() - ssstart)/25;
    progress = constrain(progress, 0, 1);

    float targetRad = (1 - progress) * data.radiation;
    currentRad += 0.1 * (targetRad - currentRad);
    analogWrite(LED_TEST, currentRad);

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

  if (strcmp(STATE,"DAY")==0 && data.radiation < 1) {
    STATE = "NIGHT";
  }
  delay(20);

}

