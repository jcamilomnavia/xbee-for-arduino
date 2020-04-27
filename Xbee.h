#ifndef Xbee_h
#define Xbee_h

// include types & constants of Wiring core API
#include "Arduino.h"
#include "HardwareSerial.h"

// Bit de comienzo de la trama
#define START_BYTE 0x7e 

// Para el modo AP 1
#define DEFAULT_FRAME_ID 1
#define DEFAULT_FRAME_TYPE 1 

// Opciones de ACK para la trama
#define ACK_OPTION 0
#define DISABLE_ACK_OPTION 1

// Tamaños no variables
#define TX_64_API_LENGTH 11 //ADDR64(8), OPCION, TYPE, ID

#include <inttypes.h>

class Xbee {
  private:
    uint8_t _frameType;
    uint8_t _frameId;
    uint64_t _destinationAddress;
    uint8_t _option;
    uint8_t _payloadData[100];
    uint8_t _payloadLength;
    uint16_t _dataFrameLength;
    uint8_t _checksum;
  
  public:
  Xbee();
    void setFrameId(uint8_t frameId);
    void setDestinationAddress(uint64_t addr64);
    void setOption(uint8_t ack_option);
    void setPayload(String payload);

    uint8_t getFrameType();
    uint8_t getFrameId();
    uint8_t getOption();
    uint64_t getDestinationAddress();
    uint8_t *getPayload();
    uint8_t getPayloadSize();
    uint16_t getDataFrameLength();

    void send();
    void write(uint8_t val);

    void reset();
};
#endif