# MICO-Wetterstation
A student project with microcontroller

## short description
The topic of this project is to develop an IoT weather station. The weather station consists of the components measurement data acquisition, processing and display (see figure below). Each of the components consists of independent microcontroller(s) of the Arduino series, which are interconnected with corresponding peripherals depending on the purpose. For data exchange the IoT platform ThingSpeak from MathWorks is used. Part 1 transmits the weather data air temperature, pressure, humidity and brightness as well as the voltage of the supply battery to ThingSpeak approximately every ten minutes. Part 2 uses this data to calculate the dew point, and also provides an overall assessment of air quality in the form of a numerical value. From the battery voltage, the battery charge is calculated. The calculated as well as measured values are again transmitted to ThingSpeak. Part 3 retrieves these values as soon as they have been updated and outputs them via an LCD display.

## project overview

![image display failed](docs/project_overview.png "schematic structure of the 3-part weather station")
