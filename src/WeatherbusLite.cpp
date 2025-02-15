/*
 *
 * This file is part of the WeatherBusLite library.
 * Copyright (c) 2025 Nikolai Patrick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "WeatherBusLite.h"

// Constructor
WeatherBusLite::WeatherBusLite() {}

/** 
 * Initialize communication.
 * 
 * Inits the RS485 communication with the specified baud rate.
 * 
 * @param baudRate Baud rate for RS485 communication
 */
void WeatherBusLite::begin(uint32_t baudRate) {
    RS485.begin(baudRate);
}

/**
 * Query temperature sensor.
 * 
 * Queries the temperature sensor and returns the temperature in degrees Celsius.
 * 
 * @param temperature Temperature in degrees Celsius
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryTemp(float &temperature) {
    sendQuery("?T");
    return parseResponse('T', temperature);
}

/**
 * Query humidity sensor.
 * 
 * Queries the humidity sensor and returns the humidity in percentage.
 * 
 * @param humidity Humidity in percentage
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryHumidity(float &humidity) {
    sendQuery("?H");
    return parseResponse('H', humidity);
}

/**
 * Query pressure sensor.
 * 
 * Queries the pressure sensor and returns the pressure in hPa.
 * 
 * @param pressure Pressure in hPa
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryPressure(float &pressure) {
    sendQuery("?P");
    return parseResponse('P', pressure);
}

/**
 * Query air quality sensor.
 * 
 * Queries the air quality sensor and returns the air quality index.
 * 
 * @param airQuality Air quality index
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryAirQuality(float &airQuality) {
    sendQuery("?A");
    return parseResponse('A', airQuality);
}

/**
 * Query UV sensor.
 * 
 * Queries the UV sensor and returns the UV index.
 * 
 * @param UV UV index
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryUV(float &UV) {
    sendQuery("?U");
    return parseResponse('U', UV);
}

/**
 * Query rainfall sensor.
 * 
 * Queries the rainfall sensor and returns the cumulative rainfall since the last time it was queried in mm.
 * 
 * @param rainfall Rainfall in mm
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryRainfall(float &rainfall) {
    sendQuery("?R");
    return parseResponse('R', rainfall);
}

/**
 * Query wind speed sensor.
 * 
 * Queries the wind speed sensor and returns the wind speed in m/s.
 * 
 * @param windSpeed Wind speed in m/s
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryWindSpeed(float &windSpeed) {
    sendQuery("?W");
    return parseResponse('W', windSpeed);
}

/**
 * Query wind direction sensor.
 * 
 * Queries the wind direction sensor and returns the wind direction in degrees.
 * 
 * @param windDirection Wind direction in degrees
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryWindDirection(float &windDirection) {
    sendQuery("?D");
    return parseResponse('D', windDirection);
}

/**
 * Query canopy temperature sensor.
 * 
 * Queries the canopy temperature sensor and returns the canopy temperature in degrees Celsius.
 * 
 * @param canopyTemperature Canopy temperature in degrees Celsius
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryCanopyTemperature(float &canopyTemperature) {
    sendQuery("?C");
    return parseResponse('C', canopyTemperature);
}

/**
 * Run a custom query.
 * 
 * Run a custom query and return the value.
 * 
 * @param queryType The type of query to run
 * @param value The value of the query
 * @return True if query was successful, false otherwise
 */
bool WeatherBusLite::queryCustom(char queryType, float &value) {
    char query[3] = {'?', queryType, '\0'};
    sendQuery(query);
    return parseResponse(queryType, value);
}

/**
 * Send query to sensor.
 * 
 * Sends a query to the bus.
 * 
 * @param query The query to send
 */
void WeatherBusLite::sendQuery(const char* query) {
    RS485.beginTransmission();
    RS485.print(query);
    RS485.println();
    RS485.endTransmission();
    RS485.flush();
    delay(WEATHERBUSLITE_GRACE);
}

/**
 * Parse response from sensor.
 * 
 * Parses the response from the sensor.
 * 
 * @param expectedType The expected type of the response
 * @param value The value of the response
 * @return True if parsing was successful, false otherwise
 */
bool WeatherBusLite::parseResponse(char expectedType, float &value) {
    enum ParseState { WAIT_FOR_START, READ_PAYLOAD, DONE };
    ParseState state = WAIT_FOR_START; // Start in the waiting state

    char response[32] = {0};  // Buffer for incoming data
    int index = 0;            // Response buffer index
    unsigned long startMillis = millis();
    RS485.receive();  // Enable receiving
    while (millis() - startMillis < WEATHERBUSLITE_RESPONSE_TIMEOUT) {
        if (RS485.available()) {
            char incoming = RS485.read();
            // State machine to parse response
            switch (state) {
                case WAIT_FOR_START:
                    if (incoming == expectedType) {  // Check if first char matches expected type (T, H, etc.)
                        response[index++] = incoming;
                        state = READ_PAYLOAD;
                    }
                    break;

                case READ_PAYLOAD:
                    if (incoming == '\n' || index >= sizeof(response) - 1) {
                        response[index] = '\0';  // Null-terminate the string
                        state = DONE;
                    } else {
                        response[index++] = incoming;
                    }
                    break;

                case DONE:
                    break;
            }

            if (state == DONE) {
                char *colonPos = strchr(response, ':');  // Find colon in response
                if (colonPos != nullptr) {
                    value = atof(colonPos + 1);  // Convert string after ':' to float
                    return true;  // Successfully parsed
                }
            }
        }
    }

    RS485.noReceive();  // Disable receiving

    return false; // Timeout or invalid response
}

