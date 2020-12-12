/**
 * @author      : worldpotato (worldpotato@$HOSTNAME)
 * @file        : wifi_connection
 * @created     : Friday Nov 20, 2020 09:06:35 CET
 */

#ifndef WIFI_CONNECTION_H

#define WIFI_CONNECTION_H


#include "../credentials/wifi_credentials.h"

namespace wifi_connection{
    WiFiClient client;

    void connectToWifi(){
        Serial.begin(115200);

        // Set WiFi to station mode and disconnect from an AP if it was previously connected
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);

        Serial.println("Setup done");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

    }
}

#endif /* end of include guard WIFI_CONNECTION_H */

