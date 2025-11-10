#include <time.h>
#include <Arduino_JSON.h>
#include <config.h>
#include <Weather_API.h>

SunData apiCall(){

    SunData data;

    http.begin(client, WAPI);
    int code = http.GET();

    if(code == 200){
        String Rawdata = http.getString();
        JSONVar document = JSON.parse(Rawdata);

        if(JSON.typeof(document)!= "undefined"){

            String sr = (const char*)document["daily"]["sunrise"][0];
            int pos = sr.indexOf("T");
            String srtime = sr.substring(pos+1);
            int h1 = srtime.substring(0,2).toInt();
            int m1 = srtime.substring(3,5).toInt();
            data.sunrise = h1*60+m1;

            String ss = (const char*)document["daily"]["sunset"][0];
            int pos2 = ss.indexOf("T");
            String sstime = ss.substring(pos2+1);
            int h2 = sstime.substring(0,2).toInt();
            int m2 = sstime.substring(3,5).toInt();
            data.sunset = h2*60+m2;

            data.radiation = (int)document["current"]["shortwave_radiation"];
        }

        else Serial.println("Error: Undefined Data");
    }

    else Serial.println("Error Fetching Data");

    http.end();
    return data;
}

int currentTime(){

    time_t now = time(nullptr);
    struct tm* CurrTime = localtime(&now);

    int finaltime = CurrTime->tm_hour * 60 + CurrTime->tm_min;
    return finaltime;
}

int todayDay() {
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    return t ? t->tm_yday : -1;    // 0..365
}

