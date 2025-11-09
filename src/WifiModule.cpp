#include <ESP8266WiFi.h>
#include <config.h>

void setupWifi(){

    WiFi.hostname("ESP8266 of AFSAL");
    WiFi.mode(WIFI_STA);

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    delay(500);
    WiFi.begin(SSID, PASS);
}

bool isWifiConnected(){
    if (WiFi.status() == WL_CONNECTED)
        return true;
    else 
        return false;
}
