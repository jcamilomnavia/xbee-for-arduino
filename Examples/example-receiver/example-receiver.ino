#include <Xbee.h>
Xbee xbee = Xbee();

void setup(){
  Serial.begin(9600);
  Serial3.begin(9600);

  delay(500);
  
  xbee.receive();
  xbee.writeDecode();
}

void loop(){

}
