#include "Xbee.h"

Xbee::Xbee(){
  Serial.begin(9600);
  _frameId = DEFAULT_FRAME_ID;
  _frameType = DEFAULT_FRAME_TYPE;
  _dataFrameLength = TX_64_API_LENGTH;
  _option = DISABLE_ACK_OPTION;
}

void Xbee::setFrameId(uint8_t frameId) {
  _frameId = frameId;
}

void Xbee::setDestinationAddress(uint64_t addr64) {
  _destinationAddress = addr64;
}

void Xbee::setOption(uint8_t ack_option) {
  _option = ack_option;
}

void Xbee::setPayload(string payload) {
    for (int i = 0; i < payload.length(); i++) { 
        _payloadData[i] = payload[i];
    }
    _dataFrameLength+= payload.length();
}

uint8_t Xbee::getFrameType(){
  return _frameType;
}

uint8_t Xbee::getFrameId(){
  return _frameId;
}

uint64_t Xbee::getDestinationAddress(){
  return _destinationAddress;
}

uint8_t Xbee::getOption(){
  return _option;
}

uint8_t *Xbee::getPayload(){
  return _payloadData;
}

uint8_t Xbee::getDataFrameLength(){
  return _dataFrameLength;
}

void Xbee::send(){
  // Se empieza una nueva trama
  write(START_BYTE);

  // Se envian los bits del tamaño de trama
  uint8_t msbLen = (getDataFrameLength() >> 8) & 0xff; // Most Significant Byte
  uint8_t lsbLen = (getDataFrameLength()) & 0xff; // Less Significant Byte
  write(msbLen);
  write(lsbLen);

  // Tipo de trama & ID de trama
  write(getFrameType());
  write(getFrameId());

  _checksum = 0;

  //Se empieza a calcula el checksum checksum, que empieza desde el Tipo de trama
  _checksum+= getFrameType();
  _checksum+= getFrameId();

  //Enviar Direccion destino y sumar al checksum
  write(getDestinationAddress());
  _checksum+= getDestinationAddress();

  // Enviar opciones y sumar al checksum
  write(getOption());
  _checksum+= getOption();

  // Enviar Payload y sumar al checksum
  // get array of message payload
  for(int i = 0; i < 7; i++){

    _checksum+= _payloadData[i];
  }

  // Se realiza el 2do complemento para el checksum
  _checksum = 0xff - _checksum;

  // Envia el checksum para finalizar la trama
  write(_checksum);

  // Se resetean los valores para un nuevo request
  reset();
}

void Xbee::write(uint8_t val) {
  Serial.write(val);
}

void Xbee::reset(){
  _frameId = DEFAULT_FRAME_ID;
  _frameType = DEFAULT_FRAME_TYPE;
  _dataFrameLength = TX_64_API_LENGTH;
  _option = DISABLE_ACK_OPTION;
  // _payloadData = ; reset to nothing
}

