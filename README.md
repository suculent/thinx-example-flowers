# thinx-example-flowers

Example of using THiNXLib for Arduino with WiFi/ESP8266 to monitor flower humidity for visualising e.g. with InfluxDB and Grafana.

##  Requirements:

  THiNXLib for Arduino (you can get it using Arduino Library Manager)

* ADS1115 16bit 4-channel ADC
* up-to 4x analog humidity sensor (e.g. with FlyingFish interface)
* MQTT broker (e.g. mosquito)
* Account at rtm.thinx.cloud

Tested with NodeMCU Lolin board - ESP8266/ESP12F.

## WiFi Setup

There's a WiFiManager built inside THiNXLib, that you can use to initialize WiFi.
Otherwise do WiFi.begin(ssid, password) once before initializing THiNXLib.

## One-channel use

In case you don't have the ADC, you can use any analog pin available on your device to test with one humidity sensor only.
You'll just need to update code to return percentage values accordingly.

## Data Visualisation

In case you want to store the data in time, you can use the mqtt-transformer node.js app to scrape your data from MQTT broker and forward it to an instance of InfluxDB (you can install it easily with Docker).
Visualise the data using Grafana. There's a nice article on that by [Phil Hawthorne](https://philhawthorne.com/getting-started-with-grafana-influxdb-for-home-assistant/)
