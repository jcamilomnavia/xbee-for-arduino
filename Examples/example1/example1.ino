#include <Xbee.h>
Xbee xbee = Xbee();

void setup(){
  Serial.begin(9600);
  xbee.setPayload("Hola mundo");
  xbee.setDestinationAddress(0xAABBCCDDEEFF1122);
  xbee.send();
}

void loop(){
  
}