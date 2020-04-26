#include <Xbee.h>
Xbee xbee = Xbee();

void setup(){
  
}

void loop(){
  xbee.setPayload("Hola mundo");
  xbee.setDestinationAddress(0000000000000000);
  xbee.send();
}