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

class Xbee {
  private:
    // Variables para transmision de paquetes

    /**
     * @brief Tipo de la trama.
     * 
     * Tipo de la trama. Por defecto sera tipo 10. Es probable que con
     * otro tipo de tramas la libreria no funcione.
     */
    uint8_t _frameType;
    /**
     * @brief ID de la trama.
     * 
     * ID de la trama. Por defecto sera 1. Si es cero, generara error
     */
    uint8_t _frameId;
    /**
     * @brief Direcciond de destino
     * 
     * Direccion de destino. Debe ser de 64 bits
     */
    uint64_t _destinationAddress;
    /**
     * @brief Direccion de 16 bits
     * 
     * Direccion de destino de 16bits(lower).
     */
    uint16_t _destinationAddress16;
    /**
     * @brief Numero de saltos
     * 
     * Broadcast Radius. Define el numero de saltos de la trama.
     * Si se establece a cero(por defecto) se obtendra el maximo numero de saltos.
     */
    uint8_t _broadcastRadius;
    /**
     * @brief Opciones de la trama.
     * 
     * Opciones de la trama. Entre sus opciones estan: 0x00, 0x01, 0x20, 0x40.
     * Referirse a documentacion de Digi para mas informacion
     */
    uint8_t _option;
    /**
     * @brief Payload de la trama a enviar
     * 
     * Payload como un paquete RF.
     * Su tamano maximo en la version S1 era de 100 bytes
     */
    uint8_t _payloadData[100];
    uint8_t _payloadLength;
    uint16_t _dataFrameLength;
    uint8_t _checksum;

    // Variables para recepción de paquetes

    /**
     * @brief MSB de la trama recibida
     * 
     * Bit mas significativo de la trama recibida
     */
    uint8_t _msbLength;
    /**
     * @brief LSB de la trama recibida
     * 
     * Bit menos significativo de la trama recibida
     */
    uint8_t _lsbLength;
    uint8_t _addressReceived[8];
    uint8_t _addressReceived16[2];
    bool _hasError;
    bool _complete;
    String _error;
    /**
     * @brief Posicion de lectura
     * 
     * Posicion de lectura de la trama recibida
     */
    uint8_t _pos;
    /**
     * @brief Siguiente Byte de lectura
     * 
     * Ultimo byte leido del Serial de la trama recibida
     */
    uint8_t b;

  public:
    /**
     * @brief Constructor de la clase
     * 
     * Como minimo se debe establecer un payload y una direccion destino para generar una trama.
     * Es opcional establecer un frameID, options y broadcast Radius
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
     * @brief Codifica la trama
     * 
     * Codifica la trama calculando el tamaño y su checksum con los valores
     * anteriormente establecidos por el usuario tales como payload, options,
     * direccion destino, entre otros.
     */
    void send();
    /**
     * @brief Escribir variables en serial
     * 
     * Escribe las variables recibidas para formar la trama API.
     * Su escritura es en HEX Hexadecimal
     * 
     * @param val Valor a escribir en el Serial
     */
    void write(uint8_t val);
    /**
     * @brief Resetea los valores iniciales de la trama
     * 
     * Re-inicializa los valores iniciales de la trama tales como:
     * Opciones, FrameID, FrameType, Direccion destino 16 bits, Broadcast Radius
     * y el tamaño de datos de la trama
     * 
     */
    void reset();

    // Metodos para recepcion de tramas

    /**
     * @brief Establece el MSB del tamaño de la trama
     * 
     * Establece el bit mas significativo (MOST SIGNIFICANT BIT) del tamaño 
     * de la trama recibida
     * 
     * @param msb Bit mas significativo
     */
    void setmsbLength(uint8_t msb);
    /**
     * @brief Establece el LSB del tamaño de la trama
     * 
     * Establece el bit menos significativo (LESS SIGNIFICANT BIT) del tamaño 
     * de la trama recibida
     * 
     * @param lsb Bit menos significativo
     */
    void setlsbLength(uint8_t lsb);
    /**
     * @brief Establece el mensaje de error
     * 
     * Establece el mensaje de error que se genera al leer
     * una trama recibida. Si hay mensaje de error, se establecera
     * un booleano true para luego ser usado en @writeDecode y mostrar
     * el mensaje de error.
     * 
     * @param error Mensaje a establecer
     */
    void setError(String error);
    /**
     * @brief Obtener el tamaño de la trama recibida
     * 
     * Retorna el tamaño de la trama recibida teniendo encuenta
     * su LSB y MSB
     * 
     * @return uint16_t Tamaño total de la trama recibida
     */
    uint16_t getPacketReceivedLength();
    uint8_t getmsbLength();
    uint8_t getlsbLength();
    uint8_t getError();

    /**
     * @brief Decodifica una trama del Serial
     * 
     * Lee los Bytes que esten disponibles en el serial y los decodifica 
     * por las partes definidas de la trama. En caso de que haya error, se
     * creara y se mostrara en el @writeDecode()
     */
    void receive();
    /**
     * @brief Inidica si hay datos por leer en Serial.
     * 
     * Retorna un booleano si el Serial ha recibido alguna trama
     * y esta puede ser leida.
     * 
     * @return true Si hay datos por leer en Serial.
     * @return false Si no hay datos por leer en Serial.
     */
    bool available();
    /**
     * @brief Lee el siguiente byte del serial.
     * 
     * @return uint8_t Byte leído del serial
     */
    uint8_t read();
    /**
     * @brief Imprime en serial la trama recibida
     * 
     * Imprime en serial la trama recibida y decodificada. Es separada en las diferentes partes
     * que compone una trama y se muestra cada seccion y su dato.
     */
    void writeDecode();
};
#endif