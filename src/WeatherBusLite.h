#ifndef WEATHERBUSLITE_H
#define WEATHERBUSLITE_H

#include <Arduino.h>
#include <ArduinoRS485.h>
#include <SoftwareSerial.h> 


// timings
#define WEATHERBUSLITE_BAUDRATE 9600
#define WEATHERBUSLITE_RESPONSE_TIMEOUT 1000
#define WEATHERBUSLITE_GRACE 2

class WeatherBusLite {
public:
    WeatherBusLite();

    void begin(uint32_t baudRate = 9600);

    bool queryTemp(float &temperature);
    bool queryHumidity(float &humidity);
    bool queryPressure(float &pressure);
    bool queryAirQuality(float &airQuality);
    bool queryUV(float &uv);
    bool queryRainfall(float &rainfall);
    bool queryWindSpeed(float &windSpeed);
    bool queryWindDirection(float &windDirection);
    bool queryCanopyTemperature(float &canopyTemperature);
    bool queryCustom(char queryType, float &value);

private:
    void sendQuery(const char* query);
    bool parseResponse(char expectedType, float &value);
};

#endif
