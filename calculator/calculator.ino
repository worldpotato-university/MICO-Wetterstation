// use this define to compile the debug messages, too
/* #define DEBUG */


#include "WiFi.h"
#include "ThingSpeak.h"

#include "lib/wifiConnection.h"
#include "lib/thingSpeakConnection.h"

#include "lib/estimation.h"

// setting up the wifi connection and the thingspeak connection
void setup()
{
    wifi_connection::connectToWifi();
    thingSpeak_connection::setupThinkSpeak();
}

void loop()
{
    // only do it if the wifi is connected
    if(WiFi.status() == WL_CONNECTED){

        // get the sensor data from thingspeak
        float sensorData[5];
        thingSpeak_connection::getSensorData(sensorData); // returns data in the sensorData array call by refenrece

        // calculate the additional data
        float taupunkt = estimation::estimateTaupunkt(&sensorData[0], &sensorData[1]);
        float quality = estimation::estimateQuality(sensorData);
        float battery = estimation::estimateAkkuProzent(sensorData[4]);

        // add the new data to a array with the old data
        float estimatedData[] = {sensorData[0], sensorData[1], sensorData[2], sensorData[3], sensorData[4], quality, taupunkt, battery};

        // update the thingspeak channel
        int updated = thingSpeak_connection::setEstimationData(estimatedData);
        #ifdef DEBUG
        if (updated == 200)
        Serial.println("Channel update successfull");
        #endif
    }
    // wait for 15 seconds because we are are only allowed to update the channel every 15 seconds
    delay(15*1000);
}

