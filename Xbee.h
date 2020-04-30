#ifndef Xbee_h
#define Xbee_h

// core Arduino
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

/**
 * Clase principal para el manejo de Xbee
 */
class Xbee {
  private:
    // Variables para transmision de paquetes
    /** 
     * Tipo de la trama. Por defecto sera tipo 10.
     */
    uint8_t _frameType;
    /** 
     * ID de la trama. Por defecto sera 1. Si es cero, generara error
     */
    uint8_t _frameId;
    /** 
     * Direccion de destino. Debe ser de 64 bits
     */
    uint64_t _destinationAddress;
    /** 
     * Direccion de destino de 16bits(lower).
     */
    uint16_t _destinationAddress16;
    /** 
     * Broadcast Radius. Define el numero de saltos de la trama.
     */
    uint8_t _broadcastRadius;
    /** 
     * Opciones de la trama. 0x00, 0x01, 0x20, 0x40
     */
    uint8_t _option;
    /** 
     * Payload como un paquete RF. Su tamano maximo en la version S1 era de 100 bytes
     */
    uint8_t _payloadData[100];
    uint8_t _payloadLength;
    uint16_t _dataFrameLength;
    uint8_t _checksum;

    // Variables para recepcion de paquetes
    /** 
     * Determina el tamaño de la trama. Most Signficant Bit
     */
    uint8_t _msbLength;
    /** 
     * Determina el tamaño de la trama. Less Signficant Bit
     */
    uint8_t _lsbLength;
    uint8_t _addressReceived[8];
    uint8_t _addressReceived16[2];
    bool _hasError;
    String _error;
    /** 
     * Posicion de lectura de bytes recibidos.
     */
    uint8_t _pos;
    /**
     * Ultimo byte leido en el serial
     */
    uint8_t b;

  public:
    /** 
     * Constructor de la clase
     */
    Xbee();

    // Metodos para envio de tramas
    void setFrameType(uint8_t frameType);
    void setFrameId(uint8_t frameId);
    void setDestinationAddress(uint64_t addr64);
    void setDestinationAddress16(uint16_t addr16);
    void setBroadcastRadius(uint8_t broadcast);
    void setOption(uint8_t ack_option);
    void setPayload(String payload);
    void setDataFrameLength(uint16_t frameLength);

    uint8_t getFrameType();
    uint8_t getFrameId();
    uint8_t getBroadcastRadius();
    uint8_t getOption();
    uint64_t getDestinationAddress();
    uint16_t getDestinationAddress16();
    uint8_t *getPayload();
    uint8_t getPayloadSize();
    uint16_t getDataFrameLength();

    /**
     * Metodo para construir la trama segun parametros establecidos
     */
    void send();
    /**
     * Metodo para escribir en el serial la trama. En hexadecimal
     */
    void write(uint8_t val);
    /**
     * Volver a los valores iniciales por defecto.
     */
    void reset();

    // Metodos para recepcion de tramas

    /**
     * Metodo para establecer el bit mas significativo del tamaño de la trama
     */
    void setmsbLength(uint8_t msb);
    /**
     * Metodo para establecer el bit menos significativo del tamaño de la trama
     */
    void setlsbLength(uint8_t lsb);
    /**
     * Metodo para establecer algun error, en caso de que haya.
     */
    void setError(String error);

    uint8_t getmsbLength();
    uint8_t getlsbLength();
    uint8_t getError();

    /**
     * Metodo que decodifica la trama por partes y crea las variables
     */
    void receive();
    /**
     * Metodo para checkear si  hay datos en el serial por ser leidos.
     */
    bool available();
    /**
     * Metodo para leer el serial.
     */
    uint8_t read();
    /**
     * Escribe separadamente las partes de la trama decodificada.
     */
    void writeDecode();
};
#endif