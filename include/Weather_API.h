#ifndef WEATHER_API_H
#define WEATHER_API_H

struct SunData{
    int sunrise;
    int sunset;
    int radiation;
};

SunData apiCall();
int currentTime();
int todayDay();

#endif