#include "Adafruit_AM2320.h"
#include <Wire.h>
#include <SFE_BMP180.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoLowPower.h>
#include <ThingSpeak.h>

#include "arduino_secrets.h"
//---------------------------------------defines----------------------------------------------------------------------------------------------------
//#define DEBUG //comment it out if no debug information should be displayed or the board should run independently.
#define DEEP_SLEEP //if you want save power to use the application with a battery define this

#define ALTITUDE 400.0 //Altitude of the wether station
#define PUBLISH_TIME 600000 //The time (ms) in which the Arduino sleeps until it wakes up again, reconnects if necessary and publishes the next values.
#define ANALOG_HUMIDITY_IN A6 //The pin where the photodiode is pluged in
#define SENSOR_VCC 7 //with this pin the photodiode are supplied (Attention max. 7mA!!!) to power it only when in use and thus save energy.
//---------------------------------------end defines------------------------------------------------------------------------------------------------
 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient client; // object of the Ethernet client library

unsigned long myChannelNumber = SECRET_CHANNEL_ID; //ChannelNumber of the ThingSpeak-Channel
const char * myWriteAPIKey = SECRET_WRITE_API_KEY; //Write-API-Key of the ThingSpeak-Channel

Adafruit_AM2320 am2320 = Adafruit_AM2320(); //object of the temperature and humidity sensor AM2320
SFE_BMP180 bmp180; //object of the air pressure and temperature sensor bmp180

//--------------------------------------------------------------setup-------------------------------------------------------------------------------------
void setup() {
  delay(15000);//wait some time to be able to update the program before the Arduino goes into sleep mode
  #ifdef DEBUG
    Serial.begin(115200);
    unsigned long t = millis();
    while (!Serial and millis()<t+30000); //Wait until the serial connection is started or the timeout of 30s is reached
  #endif
  analogReadResolution(12); //Set the resolution of the ADC to 12 Bit
  pinMode(SENSOR_VCC, OUTPUT);
  digitalWrite(SENSOR_VCC,LOW);

  am2320.begin(); //initialize the temperature and humidity sensor AM2320
  bmp180.begin(); //initialize the air pressure and temperature sensor bmp180

  if (WiFi.status() == WL_NO_MODULE) { //check if the wifi module is working
    #ifdef DEBUG 
      Serial.println("Communication with WiFi module failed!");
    #endif
    // don't continue
    while (true);
  }
  ThingSpeak.begin(client);  //initialize ThingSpeak
}
//----------------------------------------------------------end setup-------------------------------------------------------------------------------------

//-------------------------------------------------------------loop---------------------------------------------------------------------------------------
void loop() {
    if (WiFi.status() != WL_CONNECTED) WifiConnect(); //Connect the Wifi if necessary
    PublishFeed(); //Publish the next values to ThingSpeak
    #ifdef DEEP_SLEEP
      #ifdef DEBUG
        Serial.println("go sleeping");
      #endif
      WiFi.end(); //Switch off the Wifi to save energy
      LowPower.deepSleep(PUBLISH_TIME); //Set the deepSleep mode for the arduino in order to save energy
    #else
      #ifdef DEBUG
        Serial.println("delay");
      #endif
      delay(PUBLISH_TIME); //wait before publish the next values
    #endif
}
//----------------------------------------------------------end loop---------------------------------------------------------------------------------------

//-------------------------------------------------------------WifiConnect---------------------------------------------------------------------------------
void WifiConnect(){
  // attempt to connect to Wifi network:
  WiFi.lowPowerMode(); //set the low power mode for the wifi module to save energy
  while (WiFi.status() != WL_CONNECTED) { //try to connect the wifi until it works
    #ifdef DEBUG
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
    #endif
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    // wait 3 seconds for connection:
    delay(3000);
  }

  #ifdef DEBUG
    Serial.println("Connected to wifi");
    printWifiStatus();
  #endif
}
//----------------------------------------------------------end WifiConnect---------------------------------------------------------------------------------

//-------------------------------------------------------------printWifiStatus-------------------------------------------------------------------------------
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
//---------------------------------------------------------end printWifiStatus-------------------------------------------------------------------------------

//-------------------------------------------------------------PublishFeed-----------------------------------------------------------------------------------
void PublishFeed() {
  //Switch on the power supply for the sensors and wait a short time to ensure that they have started correctly.
  digitalWrite(SENSOR_VCC, HIGH);
  delay(500);
  
  //read the values of the AM2320
  float amTemp = am2320.readTemperature();
  float Humidity = am2320.readHumidity();

  //read the values of the BMP180
  float bmpTemp, Pres;
  bmp180_measurement(bmpTemp, Pres, ALTITUDE);

  //read the value of the photodiode
  int Lum = 4096 - analogRead(ANALOG_HUMIDITY_IN);

  //Switch off the power supply of the sensors
  digitalWrite(SENSOR_VCC, LOW);
  
  //read the battery voltage
  int batteryValue = analogRead(ADC_BATTERY);


  // Convert the analog reading (which goes from 0 - 4096) to a voltage (0 - 4.3V):
  float batteryVoltage = batteryValue * (4.3 / 4096.0);
  //Calculate the two temperature sensors to one value by averaging
  float Temp = (amTemp+bmpTemp)/2;

  #ifdef DEBUG
    Serial.println("Temp, Hum, Pres, Lum, battery");
    Serial.print(Temp);
    Serial.print(", ");
    Serial.print(Humidity);
    Serial.print(", ");
    Serial.print(Pres);
    Serial.print(", ");
    Serial.print(Lum);
    Serial.print(", ");
    Serial.println(batteryVoltage);
  #endif

  //set the ThingsSpeak-fields with the values
  ThingSpeak.setField(1, Temp);
  ThingSpeak.setField(2, Humidity);
  ThingSpeak.setField(3, Pres);
  ThingSpeak.setField(4, Lum);
  ThingSpeak.setField(5, batteryVoltage);

  
  int answer = 0; //answer of the ThingSpeak publishing
  byte i = 0; //trys of publishing
  while (answer != 200 and i < 30){ //try until it works or max 30 times (mayby the wifi has to connect again)
    answer = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); //write to the ThingSpeak channel
    i++;
    #ifdef DEBUG
      if(answer == 200){
        Serial.println("Channel update successful.");
      }
      else{
        Serial.println("Problem updating channel. HTTP error code " + String(answer));
      }
    #endif
  }
}
//---------------------------------------------------------end PublishFeed-----------------------------------------------------------------------------------

//---------------------------------------------------------bmp180_measurement---------------------------------------------------------------------------------
bool bmp180_measurement(float &T, float &P,float Altitude)
{
  char status; //the functions returns a status char
  
  status = bmp180.startTemperature(); //initialize the temperature measurement
  if(status != 0)
  {
    delay(status); //Wait for the measurement to complete
    status = bmp180.getTemperature(T); //get the temperature value
    if(status != 0)
    {
      //initialize the pressure measurement; the parameter from 0 to 3 sets the resolution
      // 0:low res. - 3:high res. but the higher the resolution the longer the measurement takes 
      status = bmp180.startPressure(3); 
      if (status != 0)
      {
        delay(status); //Wait for the measurement to complete
        status = bmp180.getPressure(P,T); //get the pressure value
        if (status != 0)
        {
          P = bmp180.sealevel(P,Altitude)*100; //Calculate the reference pressure in relation to sea level (1013.25 hPa) and convert to Pascal
        }
        #ifdef DEBUG
        else Serial.println("error retrieving pressure measurement\n");
        #endif
      }
      #ifdef DEBUG
      else Serial.println("error starting pressure measurement\n");
      #endif
    }
    #ifdef DEBUG
    else Serial.println("error retrieving temperature measurement\n");
    #endif
  }
  #ifdef DEBUG
  else Serial.println("error starting temperature measurement\n");
  #endif
  
  return status > 0; //return whether the sensor measurement was successful
}
//-----------------------------------------------------end bmp180_measurement---------------------------------------------------------------------------------
