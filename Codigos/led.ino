#include <MySigningNone.h>
#include <MyTransportNRF24.h>
#include <MyTransportRFM69.h>
#include <MyHwATMega328.h>
#include <MySensor.h>
#include <SPI.h>

#define RELAY_1  3  
#define NUMBER_OF_RELAYS 1 
#define RELAY_ON 1  
#define RELAY_OFF 0 


MyTransportNRF24 radio(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);  

MyHwATMega328 hw;

MySensor gw(radio, hw);
int node_id = 3;

void setup()    
{   

  gw.begin(incomingMessage, node_id, true);

  gw.sendSketchInfo("Relay", "1.0");


  for (int sensor=1, pin=RELAY_1; sensor<=NUMBER_OF_RELAYS;sensor++, pin++) {

    gw.present(sensor, S_LIGHT);

    pinMode(pin, OUTPUT);   

    digitalWrite(pin, gw.loadState(sensor)?RELAY_ON:RELAY_OFF);
  }
}


void loop() 
{
  // Alway process incoming messages whenever possible
  gw.process();
}

void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_LIGHT) {
     // Change relay state
     digitalWrite(message.sensor-1+RELAY_1, message.getBool()?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     gw.saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}
