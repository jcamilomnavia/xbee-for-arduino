#include <Xbee.h>
Xbee xbee = Xbee();

void setup(){
  Serial.begin(9600);
  delay(3000);
  xbee.receive();
  xbee.writeDecode();
}

void loop(){

}
