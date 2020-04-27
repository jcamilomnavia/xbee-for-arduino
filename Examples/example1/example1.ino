#include <Xbee.h>
Xbee xbee = Xbee();

void setup(){
  Serial.begin(9600);
  xbee.setPayload("Hola mundo");
  xbee.setDestinationAddress(0000000000000000);
  xbee.send();
}

void loop(){
  
}