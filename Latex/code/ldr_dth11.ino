#include <SPI.h>
#include <MySensor.h>  
#include <DHT.h>  

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define HUMIDITY_SENSOR_DIGITAL_PIN 3

#define CHILD_ID_LIGHT 3
#define LIGHT_SENSOR_ANALOG_PIN 0

unsigned long SLEEP_TIME = 30000; // Sleep time between reads (in milliseconds)

MySensor gw;

MyMessage msg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
int lastLightLevel;

DHT dht;
float lastTemp;
float lastHum;
boolean metric = true; 
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
int node_id = 2;

void setup()  
{ 
  gw.begin(NULL, node_id);

  gw.sendSketchInfo("Light Sensor", "1.0");
  gw.present(CHILD_ID_LIGHT, S_LIGHT_LEVEL);

  dht.setup(HUMIDITY_SENSOR_DIGITAL_PIN); 

  // Send the Sketch Version Information to the Gateway
  gw.sendSketchInfo("Humidity", "1.0");

  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID_HUM, S_HUM);
  gw.present(CHILD_ID_TEMP, S_TEMP);
  
  metric = gw.getConfig().isMetric;

  
}

void loop()      
{  

  int lightLevel = (1023-analogRead(LIGHT_SENSOR_ANALOG_PIN))/10.23; 
  Serial.println(lightLevel);
  if (lightLevel != lastLightLevel) {
      gw.send(msg.set(lightLevel));
      lastLightLevel = lightLevel;
  }
  
  delay(dht.getMinimumSamplingPeriod());

  float temperature = dht.getTemperature();
  if (isnan(temperature)) {
      Serial.println("Failed reading temperature from DHT");
  } else if (temperature != lastTemp) {
    lastTemp = temperature;
    if (!metric) {
      temperature = dht.toFahrenheit(temperature);
    }
    gw.send(msgTemp.set(temperature, 1));
    Serial.print("T: ");
    Serial.println(temperature);
  }
  
  float humidity = dht.getHumidity();
  if (isnan(humidity)) {
      Serial.println("Failed reading humidity from DHT");
  } else if (humidity != lastHum) {
      lastHum = humidity;
      gw.send(msgHum.set(humidity, 1));
      Serial.print("H: ");
      Serial.println(humidity);
  }

  gw.sleep(SLEEP_TIME); //sleep a bit
}
