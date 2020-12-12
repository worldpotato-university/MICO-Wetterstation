/**
 * @author      : worldpotato (worldpotato@$HOSTNAME)
 * @file        : thingSpeak_connection
 * @created     : Friday Nov 20, 2020 09:24:29 CET
 */

#ifndef THINGSPEAK_CONNECTION_H

#define THINGSPEAK_CONNECTION_H

#include "../credentials/thingSpeakWettereinschaetzung.h"
#include "../credentials/thingSpeakSensorChannel.h"

namespace thingSpeak_connection {
    void setupThinkSpeak(){
        ThingSpeak.begin(wifi_connection::client);
    }

    void getSensorData(float* ret){
        float temperature = ThingSpeak.readFloatField(SENSOR_DATA_CHANNEL_ID, 1);
        float luftfeuchte = ThingSpeak.readFloatField(SENSOR_DATA_CHANNEL_ID, 2);
        float luftdruck = ThingSpeak.readFloatField(SENSOR_DATA_CHANNEL_ID, 3);
        float helligkeit = ThingSpeak.readFloatField(SENSOR_DATA_CHANNEL_ID, 4);
        float battery = ThingSpeak.readFloatField(SENSOR_DATA_CHANNEL_ID, 5);

        #ifdef DEBUG
            Serial.println(temperature);
            Serial.println(luftfeuchte);
            Serial.println(luftdruck);
            Serial.println(helligkeit);
            Serial.println(battery);
        #endif

        ret[0] = temperature;
        ret[1] = luftfeuchte;
        ret[2] = luftdruck;
        ret[3] = helligkeit;
        ret[4] = battery;
    }

    int setEstimationData(float* data){
        int numberOfData = 8;

        // set the data
        for (int i = 1; i <= numberOfData; i++){
            ThingSpeak.setField(i, data[i-1]);
            #ifdef DEBUG
                Serial.print("Set field ");
                Serial.print(i);
                Serial.print(" to ");
                Serial.println(data[i-1], 3);
            #endif
        }
        
        int i = ThingSpeak.writeFields(WETTER_EINSCHAETZUNG_CHANNEL_ID, WETTER_EINSCHAETZUNG_W_API_KEY);
    }

}

#endif /* end of include guard THINGSPEAK_CONNECTION_H */

