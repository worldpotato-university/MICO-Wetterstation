/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */

// libraries

/* #define DEBUG */

#include "WiFi.h"
#include "ThingSpeak.h"

#include "lib/wifi_connection.h"
#include "lib/thingSpeak_connection.h"

#include "lib/sensorData.h"
#include "lib/estimation.h"

// credentials

void setup()
{
    wifi_connection::connectToWifi();
    thingSpeak_connection::setupThinkSpeak();
}

void loop()
{
    // only do it if the wifi is connected
    if(WiFi.status() == WL_CONNECTED){

        float sensorData[5];
        thingSpeak_connection::getSensorData(sensorData); // returns data in the sensorData array call by refenrece

        float taupunkt = estimation::estimateTaupunkt(&sensorData[0], &sensorData[1]);
        float quality = estimation::estimateQuality(sensorData);
        float battery = estimation::estimateAkkuProzent(sensorData[4]);

        float estimatedData[] = {sensorData[0], sensorData[1], sensorData[2], sensorData[3], sensorData[4], quality, taupunkt, battery};

        int updated = thingSpeak_connection::setEstimationData(estimatedData);
        #ifdef DEBUG
        if (updated == 200)
        Serial.println("Channel update successfull");
        #endif
    }
    delay(15*1000);
}

