#include <MySensor.h>
#include <SPI.h>
#define ID 0
#define OPEN 1
#define CLOSE 0
MySensor gw;
MyMessage msg(ID, V_TRIPPED);
void setup() { 
  gw.begin();
  gw.present(ID, S_DOOR); 
}
void loop() {
     gw.send(msg.set(OPEN)); 
     delay(10000);
}