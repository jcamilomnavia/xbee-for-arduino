#include "Xbee.h"

Xbee::Xbee()
{
  Serial.begin(9600);
  _frameId = DEFAULT_FRAME_ID;
  _frameType = DEFAULT_FRAME_TYPE;
  _dataFrameLength = TX_64_API_LENGTH;
  _destinationAddress16 = DEFAULT_ADDR16;
  _broadcastRadius = BROADCAST_RADIUS;
  _option = OPTION_DEFAULT;
}

void Xbee::setFrameId(uint8_t frameId)
{
  _frameId = frameId;
}

void Xbee::setDestinationAddress(uint64_t addr64)
{
  _destinationAddress = addr64;
}

void Xbee::setDestinationAddress16(uint16_t addr16)
{
  _destinationAddress16 = addr16;
}

void Xbee::setBroadcastRadius(uint8_t broadcast)
{
  _broadcastRadius = broadcast;
}

void Xbee::setOption(uint8_t ack_option)
{
  _option = ack_option;
}

void Xbee::setPayload(String payload)
{
  for (int i = 0; i < payload.length(); i++)
  {
    _payloadData[i] = payload[i];
  }
  _payloadLength = payload.length();
  _dataFrameLength += payload.length();
}

void Xbee::setmsbLength(uint8_t msb)
{
  _msbLength = msb;
}

void Xbee::setlsbLength(uint8_t lsb)
{
  _lsbLength = lsb;
}

void Xbee::setError(String error)
{
  _error = error;
  _hasError = true;
  _complete = true;
}

uint8_t Xbee::getFrameType()
{
  return _frameType;
}

uint8_t Xbee::getFrameId()
{
  return _frameId;
}

uint64_t Xbee::getDestinationAddress()
{
  return _destinationAddress;
}

uint16_t Xbee::getDestinationAddress16()
{
  return _destinationAddress16;
}

uint8_t Xbee::getBroadcastRadius()
{
  return _broadcastRadius;
}

uint8_t Xbee::getOption()
{
  return _option;
}

uint8_t *Xbee::getPayload()
{
  return _payloadData;
}

uint16_t Xbee::getDataFrameLength()
{
  return _dataFrameLength;
}

uint8_t Xbee::getPayloadSize()
{
  return _payloadLength;
}

void Xbee::send()
{
  // Se empieza una nueva trama
  write(START_BYTE);

  // Se envian los bits del tamaño de trama
  uint8_t msbLen = (getDataFrameLength() >> 8) & 0xff; // Most Significant Byte
  uint8_t lsbLen = (getDataFrameLength()) & 0xff;      // Less Significant Byte
  write(msbLen);
  write(lsbLen);

  // Tipo de trama & ID de trama
  write(getFrameType());
  write(getFrameId());

  _checksum = 0;

  //Se empieza a calcula el checksum checksum, que empieza desde el Tipo de trama
  _checksum += getFrameType();
  _checksum += getFrameId();

  //Enviar Direccion destino 64 bits y sumar al checksum
  //(Descomponer en pares de 8 bits)
  int bits[8] = {56, 48, 40, 32, 24, 16, 8, 0};
  for (int j = 0; j < 8; j++)
  {
    uint8_t addr = (getDestinationAddress() >> bits[j]) & 0xff;
    write(addr);
    _checksum += addr;
  }

  //Enviar Direccion destino 16 bits y sumar al checksum
  //(Descomponer en pares de 8 bits)
  int bits16[2] = {8, 0};
  for (int j = 0; j < 2; j++)
  {
    uint8_t addr = (getDestinationAddress16() >> bits16[j]) & 0xff;
    write(addr);
    _checksum += addr;
  }

  // Enviar broadcast radius y sumar al checksum
  write(getBroadcastRadius());
  _checksum += getBroadcastRadius();

  // Enviar opciones y sumar al checksum
  write(getOption());
  _checksum += getOption();

  // Enviar Payload y sumar al checksum
  for (int i = 0; i < getPayloadSize(); i++)
  {
    write(_payloadData[i]);
    _checksum += _payloadData[i];
  }

  // Se realiza el 2do complemento para el checksum
  _checksum = 0xff - _checksum;

  // Envia el checksum para finalizar la trama
  write(_checksum);

  // Se resetean los valores para un nuevo request
  reset();
}

void Xbee::write(uint8_t val)
{
  if (val < 16)
  {
    Serial.print(0);
  }
  char val2 = val;
  //Serial.print(val2);
  Serial.print(val,HEX);
  Serial.print(" ");
  Serial3.write(val);
}

void Xbee::reset()
{
  _frameId = DEFAULT_FRAME_ID;
  _frameType = DEFAULT_FRAME_TYPE;
  _dataFrameLength = TX_64_API_LENGTH;
  _destinationAddress16 = DEFAULT_ADDR16;
  _broadcastRadius = BROADCAST_RADIUS;
  _option = OPTION_DEFAULT;
  // _payloadData = ; reset to nothing
}

bool Xbee::available()
{
  return Serial3.available() > 0;
}

uint8_t Xbee::read()
{
  return Serial3.read();
}

uint16_t Xbee::getPacketReceivedLength()
{
  return ((_msbLength << 8) & 0xff) + (_lsbLength & 0xff);
}

void Xbee::setDataFrameLength(uint16_t frameLength)
{
  _dataFrameLength = frameLength;
}

void Xbee::receive()
{
  _checksum = 0;
  _payloadLength = 0;

  while (available())
  {

    b = read();
    Serial.print(b,HEX);

    if (_pos > 0 && b == START_BYTE)
    {
      setError("Error: Nuevo paquete antes de finalizar el anterior");
      break;
    }

    if (_pos >= FRAME_TYPE_INDEX))
    {
       _checksum += b;
    }

    switch (_pos)
    {
    case 0:
      if (b == START_BYTE)
      {
        _pos++;
      }
      break;
    case 1:
      setmsbLength(b);
      _pos++;
      break;
    case 2:
      setlsbLength(b);
      _pos++;
      break;
    case 3:
      if(b != 0x10){
        setError("Error: Tipo de trama no soportada");
        return;
      } else {
        //setFrameType(b);
      }
      _pos++;
      break;
    case 4:
      setFrameId(b);
      _pos++;
      break;
    case 5 ... 12: 
      // 64 bit address
      _addressReceived[_pos-5] = b;
      _pos++;
      break;
    case 13 ... 14: 
      // 16 bit address
      _addressReceived16[_pos-13] = b;
      _pos++;
      break;
    case 15: 
      // broadcast radius
      setBroadcastRadius(b);
      _pos++;
      break;
    case 16: 
      // options
      setOption(b);
      _pos++;
      break;
    default:
      if (_pos > MAX_FRAME_DATA_SIZE)
      {
        setError("Error: Tamano maximo excedido");
        return;
      }

      // Se le suma 3 por el Byte delimitador, y los 2 Bytes de tamaño
      if (_pos == (getPacketReceivedLength() + 3))
      {
        if ((_checksum & 0xff) == 0xff)
        {
          // Serial.write("Checksum valido");
          _complete = true;
        }
        else
        {
          setError("Error: Checksum invalido");
          return;
        }
        // Sin contar delimitador, 2 bytes de tamaño, y checksum
        setDataFrameLength(_pos - 4);
        _pos = 0;
      }
      else
      {
        _payloadData[_pos - 17] = b;
        _payloadLength++;
	_pos++;
      }
    }
  }
  if(_complete){
    writeDecode();
  }
}


void Xbee::writeDecode()
{
  Serial.println();
  Serial.println("Trama leida:");
  if(_hasError){
    Serial.print(_error);
    return;
  }

  Serial.print("Tamaño Trama: ");
  Serial.println(getPacketReceivedLength());

  Serial.print("Frame ID: ");
  Serial.println(getFrameId());

  Serial.print("Direccion: ");
  for (int j = 0; j < 8; j++)
  {
    uint8_t addr = _addressReceived[j];
    Serial.print(addr,HEX);
  }
  Serial.println();

  Serial.print("Direccion 16 bits: ");
  for (int j = 0; j < 2; j++)
  {
    uint8_t addr16 = _addressReceived16[j];
    Serial.print(addr16,HEX);
  }
  Serial.println();

  Serial.print("BroadCast Radius: ");
  Serial.println(getBroadcastRadius(),HEX);

  Serial.print("Opciones: ");
  Serial.println(getOption(),HEX);

  Serial.print("Payload: ");
  for (int i = 0; i < getPayloadSize(); i++)
  {
    char val = _payloadData[i];
    Serial.print(val);
  }
  Serial.println();

  Serial.print("Checksum: ");
  Serial.print("Valido - ");
  Serial.println(_checksum, HEX);
  _complete = false;
}
