/*  Example of using THiNXLib to monitor flower humidity.
 *  
 *  Requirements:
 *  • ADS1115 16bit 4-channel ADC
 *  • 4x analog humidity sensor (e.g. with FlyingFish interface)
 *  • MQTT broker (e.g. mosquito)
 *  • Account at rtm.thinx.cloud
 */


#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <THiNXLib.h>
#include <ArduinoJSON.h>

THiNX thx;

bool connection_complete = false;
long nextUpdate = millis() + 3600000;

/* Called after library gets connected and registered */
void finalizeCallback () {
  connection_complete = true;
  nextUpdate = millis() + 5000;
  Serial.println("Callback completed.");
}

Adafruit_ADS1115 ads;

void setup(void) 
{
  Serial.begin(115200);
  Serial.println("Hello!");

  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV

  ads.begin();

  // API Key, Owner ID
  thx = THiNX("4721f08a6df1a36b8517f678768effa8b3f2e53a7a1934423c1f42758dd83db5", "cedc16bb6bb06daaa3ff6d30666d91aacd6e3efbf9abbc151b4dcade59af7c12");
  thx.thinx_mqtt_url = "192.168.1.21"; // redirect MQTT to local broker before thinx will attempt to connect to broker after several thx.loop() calls.
  thx.setFinalizeCallback(finalizeCallback);
}

long timeout = 0;

void send(String flower, float hum)
{
  DynamicJsonBuffer jsonBuffer(256); 
  JsonObject& root = jsonBuffer.createObject();
  
  root["shortIdentifier"] = flower;
  root["humidity"] = hum;
  
  String output;
  root.printTo(output);
  thx.publishStatusUnretained(output);
}

long measure = millis();
int16_t adc0, adc1, adc2, adc3;
double hum0, hum1, hum2, hum3;
  
void loop(void) 
{
  // measure every 5 seconds
  if (millis() > measure) {

    measure = millis() + 5000;  
         
    adc0 = ads.readADC_SingleEnded(0); delay(10);    
    adc1 = ads.readADC_SingleEnded(1); delay(10);    
    adc2 = ads.readADC_SingleEnded(2); delay(10);    
    adc3 = ads.readADC_SingleEnded(3); delay(10);
  
    Serial.print("adc0: "); Serial.println(adc0);       
    Serial.print("adc1:"); Serial.println(adc1);  
    Serial.print("adc2:"); Serial.println(adc2);
    Serial.print("adc3:"); Serial.println(adc3);
  
    double percent = 32768 / 100;
    
    hum0 = (32768 - adc0) / percent;
    hum1 = (32768 - adc1) / percent;
    hum2 = (32768 - adc2) / percent;
    hum3 = (32768 - adc3) / percent;
    
    Serial.print("F0: "); Serial.println(hum0);       
    Serial.print("F1 "); Serial.println(hum1);  
    Serial.print("F2 "); Serial.println(hum2);
    Serial.print("F2 "); Serial.println(hum3);
  }
 
  thx.loop();

  // Try to send data every 15 secs
  if (connection_complete) {
    if (millis() > timeout) {        
      send("Reference_Water_Cup", hum0);
      send("Haworthia_Cooperi", hum1);
      send("Sempervivum", hum2);
      send("Haworthiopsis_Fasciata", hum3);      
      timeout = millis() + 15000;
    }
  }
}

