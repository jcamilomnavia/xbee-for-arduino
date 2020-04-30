#ifndef Xbee_h
#define Xbee_h

// core API
#include "Arduino.h"

// Bit de comienzo de la trama
#define START_BYTE 0x7e 

// Para el modo AP 1
#define DEFAULT_FRAME_ID 0x01
#define DEFAULT_FRAME_TYPE 0x10 // Para direcciones de 64 bits, tramas tipo 10, Xbee s2

// Definicion de direccion 16bits MSB LSB
#define DEFAULT_ADDR16 0xFFFE
// #define DEFAULT_ADDR16_LSB 0xFE

// Definicion de saltos por defecto
#define BROADCAST_RADIUS 0x00

// Opciones de ACK para la trama
#define OPTION_DEFAULT 0x00

// Tamaños no variables
#define TX_64_API_LENGTH 14 //ADDR64(8), ADRR16(2), OPCION, TYPE, ID, BROADCAST-RADIUS
#define FRAME_TYPE_INDEX 3
#define MAX_FRAME_DATA_SIZE 110

#include <inttypes.h>

class Xbee {
  private:
    // Variables para transmision de paquetes
    uint8_t _frameType;
    uint8_t _frameId;
    uint64_t _destinationAddress;
    uint16_t _destinationAddress16;
    uint8_t _broadcastRadius;
    uint8_t _option;
    uint8_t _payloadData[100];
    uint8_t _payloadLength;
    uint16_t _dataFrameLength;
    uint8_t _checksum;

    // Variables para recepcion de paquetes
    uint8_t _msbLength;
    uint8_t _lsbLength;
    bool _hasError;
    uint8_t _error;
    uint8_t _pos;
    uint8_t b; // last byte read

  public:
    Xbee();
    void setFrameType(uint8_t frameType);
    void setFrameId(uint8_t frameId);
    void setDestinationAddress(uint64_t addr64);
    void setDestinationAddress16(uint16_t addr16);
    void setBroadcastRadius(uint8_t broadcast);
    void setOption(uint8_t ack_option);
    void setPayload(String payload);
    void setDataFrameLength(uint16_t frameLength);

    void setmsbLength(uint8_t msb);
    void setlsbLength(uint8_t lsb);
    void setComplete(bool complete);
    void setError(uint8_t error);

    uint8_t getFrameType();
    uint8_t getFrameId();
    uint8_t getBroadcastRadius();
    uint8_t getOption();
    uint64_t getDestinationAddress();
    uint16_t getDestinationAddress16();
    uint8_t *getPayload();
    uint8_t getPayloadSize();
    uint16_t getDataFrameLength();

    uint8_t getmsbLength();
    uint8_t getlsbLength();
    uint16_t getPacketReceivedLength();
    bool getComplete();
    uint8_t getError();

    void send();
    void write(uint8_t val);
    void receive();
    void available();
    void read()
    void writeDecode();

    void reset();
};
#endif