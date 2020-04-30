#include <Xbee.h>
Xbee xbee = Xbee();

void setup(){
  Serial.begin(9600);
  Serial3.begin(9600); // Para enviar al serial del arduino recibidor
  xbee.setPayload("Hola mundo");
  xbee.setDestinationAddress(0xAABBCCDDEEFF1122);
  xbee.send(); // 7E 00 18 10 01 AA BB CC DD EE FF 11 22 FF FE 00 00 48 6F 6C 61 20 6D 75 6E 64 6F FC
}

void loop(){
  
}
