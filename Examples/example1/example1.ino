#include <Xbee.h>
Xbee xbee = Xbee();

void setup(){
  xbee.setPayload("Hola mundo");
  xbee.setDestinationAddress(0000000000000000);
  xbee.send();
}

void loop(){
  
}