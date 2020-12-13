#include <Wire.h>
#include <WiFiNINA.h>                     
#include <PubSubClient.h> 
#include "Secrets.h"        

/* Präkompilerdefinition TEST_MODE für Testmodus 
 * entfernen um Testmodusanweisungen nicht zu kompilieren. 
 * Betrifft va. Ausgaben über serielle Schnittstelle 
 */
#define TEST_MODE   

WiFiClient client;                        // Wifi Client
PubSubClient mqttClient(client);          // MQTT Client, an Wifi gebunden


void setup() {

	#ifdef TEST_MODE
  Serial.begin(9600);
  while (!Serial) {
    ; // Warten bis Serielle Schnittstelle verbunden ist.
  }
  Serial.println("Mico-B ThingSpeak Beispiel, MQTT Subscribe");
	#endif

  // Statusinitialisierung benötigt für folgende Schritte
  int status = WL_IDLE_STATUS;

  // Verbingungsversuch mit WPA/WPA2 WIFI-Netzwerk
  while (status != WL_CONNECTED) 
  {
		#ifdef TEST_MODE
    Serial.println("Try WiFi connection ...");
		#endif
    
    status = WiFi.begin(SSID, PASS);
    delay(5000); // Warte vor einem neuen Versuch oder einem Zugriff
  }
	
	#ifdef TEST_MODE
  Serial.println("Connected to wifi");
	#endif

  mqttClient.setServer("mqtt.thingspeak.com", 1883);   
  mqttClient.setCallback(sendMessageViaI2C);

  // Setup I2C-Bus
  Wire.begin(); // Bus beitreten. Adresse optional für Master
}


void loop() {
  // Verbindungsversuch und Subscription
  if (!mqttClient.connected()) 
  {
    reconnect();
  }
  
  mqttClient.loop();   // Wiederholter Aufruf der loop-Funktion um Verbindung Aufrecht zu erhalten.
}


// MQTT Verbindung (wieder)aufbauen.
void reconnect() {
  // Loop until reconnected.
  while (!mqttClient.connected()) {
    // Zufällige Client Id aus User Name und Zufallszahl.
    String clientID = String(MQTT_USER_NAME) + "_" + String( random(), HEX );

		#ifdef TEST_MODE
    Serial.print("Attempting MQTT connection...");
    #endif

    // Verindung mit dem MQTT broker.
    if (mqttClient.connect(clientID.c_str(), MQTT_USER_NAME, MQTT_PASS)) {
			String topic = "channels/" + String( CHANNEL_ID ) + "/subscribe/fields" + "/+/" + String(READ_API_KEY);
			mqttClient.subscribe(topic.c_str());

			#ifdef TEST_MODE
      Serial.print("connected - subscribe to ");
      Serial.println(topic);
			#endif

    } else {

			#ifdef TEST_MODE
      Serial.print("failed, rc=");
      // Print reason the connection failed.
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
			#endif

      delay(5000);
    }
  }
}

void sendMessageViaI2C(char* topic, byte* payload, unsigned int length) {

  #ifdef TEST_MODE
  Serial.print("Message arrived [");
  Serial.print(topic[39]); // Feldnummer nach MQTT Protokoll im an Stelle 40
  Serial.print("] ");
  #endif

  // I2C transmit to device #8
  Wire.beginTransmission(8);
  Wire.write((char)topic[39]); // Voranstellen der Feldnummer als ID

  // payload byteweise versenden (verkettete Liste)
  for (unsigned int i=0;i<length;i++) {
    Wire.write((char)(payload[i]));
  }
  Wire.endTransmission();

  #ifdef TEST_MODE
  for (unsigned int i=0;i<length;i++) {
    Serial.print((char)(payload[i]));
  }       

  Serial.println();
  #endif
}
