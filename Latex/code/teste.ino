#define NO_PORTB_PINCHANGES  

#include <MySigningNone.h>
#include <MyTransportRFM69.h>
#include <MyTransportNRF24.h>
#include <MyHwATMega328.h>
#include <MySigningAtsha204Soft.h>
#include <MySigningAtsha204.h>

#include <SPI.h>  
#include <MyParserSerial.h>  
#include <MySensor.h>  
#include <stdarg.h>
#include <PinChangeInt.h>
#include "GatewayUtil.h"

#define INCLUSION_MODE_TIME 1 

#define INCLUSION_MODE_PIN  3 

#define RADIO_ERROR_LED_PIN 4  
#define RADIO_RX_LED_PIN    6  
#define RADIO_TX_LED_PIN    5  

 
MyTransportNRF24 transport(RF24_CE_PIN, RF24_CS_PIN, RF24_PA_LEVEL_GW);

 
MyHwATMega328 hw;


#ifdef WITH_LEDS_BLINKING
MySensor gw(transport, hw /*, signer*/, RADIO_RX_LED_PIN, RADIO_TX_LED_PIN, RADIO_ERROR_LED_PIN);
#else
MySensor gw(transport, hw /*, signer*/);
#endif

char inputString[MAX_RECEIVE_LENGTH] = "";    
int inputPos = 0;
boolean commandComplete = false;  

void parseAndSend(char *commandBuffer);

void output(const char *fmt, ... ) {
   va_list args;
   va_start (args, fmt );
   vsnprintf_P(serialBuffer, MAX_SEND_LENGTH, fmt, args);
   va_end (args);
   Serial.print(serialBuffer);
}

  
void setup()  
{ 
  gw.begin(incomingMessage, 0, true, 0);

  setupGateway(INCLUSION_MODE_PIN, INCLUSION_MODE_TIME, output);

  PCintPort::attachInterrupt(pinInclusion, startInclusionInterrupt, RISING);

  serial(PSTR("0;0;%d;0;%d;Gateway startup complete.\n"),  C_INTERNAL, I_GATEWAY_READY);
}

void loop()  
{ 
  gw.process();

  checkButtonTriggeredInclusion();
  checkInclusionFinished();
  
  if (commandComplete) {
    parseAndSend(gw, inputString);
    commandComplete = false;  
    inputPos = 0;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inputPos<MAX_RECEIVE_LENGTH-1 && !commandComplete) { 
      if (inChar == '\n') {
        inputString[inputPos] = 0;
        commandComplete = true;
      } else {
        inputString[inputPos] = inChar;
        inputPos++;
      }
    } else {
        inputPos = 0;
    }
  }
}
