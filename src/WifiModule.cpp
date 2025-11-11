#include <ESP8266WiFi.h>
#include <config.h>

//if you dont understand what is going on here.. you dumb
void setupWifi(){

    WiFi.hostname("ESP8266");
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
