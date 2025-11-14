#include <time.h>
#include <Arduino_JSON.h>
#include <config.h>
#include <Weather_API.h>
#include <main.h>

//Handles all The API thing.
SunData apiCall(){

    //Fetch the Sunrise, Sunset and Sun radiation data in JSON and returns it to Struct SunData
    SunData data;

    http.begin(client, WAPI);
    int code = http.GET();

    if(code == 200){
        String Rawdata = http.getString();
        JSONVar document = JSON.parse(Rawdata);

        if(JSON.typeof(document)!= "undefined"){

            String sr = (const char*)document["daily"]["sunrise"][0];
            data.sunrise = separator(sr, "T");

            String ss = (const char*)document["daily"]["sunset"][0];
            data.sunset = separator(ss, "T");

            data.radiation = (int)document["current"]["shortwave_radiation"];
        }

        else Serial.println("Error: Undefined Data");
    }

    else Serial.println("Error Fetching Data");

    http.end();
    return data;
}

int currentTime(){

    //Retuens current time from the API
    time_t now = time(nullptr);
    struct tm* CurrTime = localtime(&now);

    int finaltime = CurrTime->tm_hour * 60 + CurrTime->tm_min;
    return finaltime; // in minutes
}

int todayDay() {
    
    //for checking weather a sunrise or sunset happened today
    //otherwise the controller will be in limbo
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    return t ? t->tm_yday : -1;    // 0 to 365 range
}

