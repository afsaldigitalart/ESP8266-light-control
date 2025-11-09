#ifndef WEATHER_API_H
#define WEATHER_API_H

struct SunData{
    int sunrise;
    int sunset;
    int radiation;
    bool valid;
};

SunData apiCall();
int currentTime();
int todayDay();

#endif