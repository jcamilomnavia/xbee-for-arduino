# Xbee para Arduino

Esta libreria pretende transmitir & recibir tramas API tipo 10 con el modulo XBee S2, donde el usuario, para transmitir, simplemente ingresa una dirección destino de 64 bits y el mensaje a transmitir. Además podrá configurar opciones adicionales si lo desea.

Para entender mejor la libreria se explicaran las tramas API tipo 10 con direcciones de 64 bits para el modulo XBee s2.

## Instalación
Arduino incluye un manejador de librerias para una facil instalacion. Del menu Programa/Sketch selecciona ```Incluir libreria->Añadir biblioteca .ZIP``` y selecciona el archivo .zip de esta libreria. 

Una segunda opcion es arrastrar la carpeta de esta libreria manualmente a la carpeta de ```librerias``` de Arduino.

Para usar esta libreria en tu sketch deberás añadir la siguiente linea al principio de tu archivo ```.ino```

```C
#include <Xbee.h>
```

## Configuración 

Para usar esta libreria, tu XBee debe estar configurado en modo API. Para configurarlo, referirse a la documentación de XCTU y de DIGI.

## Uso

El Xbee debe estar conectado al **Serial** del Arduino. Si es conectado al Serial1 o cualquier otro, esta librería no funcionará.

### Ejemplo en Arduino

Para transmitir tramas:
```C
#include <Xbee.h>
Xbee myXbee = Xbee();

void setup(){
  Serial.begin(9600);
  myXbee.setPayload("Hola mundo");
}

void loop(){
  myXbee.setPayload("Hola mundo");
  myXbee.setDestinationAddress(0xAABBCCDDEEFF1122);
  myXbee.send();
}
```

Para recibir tramas:
```C
#include <Xbee.h>
Xbee myXbee = Xbee();

void setup(){
  Serial.begin(9600);
  myXbee.receive();
  myXbee.writeDecode();
}

void loop(){
  
}
```

## Documentación de XBee

### Especificaciones de la trama API

El XBee soporta 3 modos de operación, de los cuales 2 son en modo api. Estos se pueden configurar en la terminal del Xbee en XCTU.
Estas son los modos:

| Comando AP | Descripción                                              |
|------------|----------------------------------------------------------|
| AP = 0     | Modo transparente o AT. Esta es la configuración inicial |
| AP = 1     | Operacion API                                            |
| AP = 2     | Operacion API con caracteres de escape                   |

Las estructura de las tramas API depende del modo que se escoja. En esta libreria se supondra que trabajara en modo 1, sin caracteres de escape.

### Operación Zigbee Transmit Request. Tipo de trama 0x010

Una peticion de transmision de trama API causa que el modulo envie datos como paquetes RF al destino especificado.

La siguiente tabla muestra la estructura de la trama cuando se habilita este tipo:

| Campos de la trama        | Byte | Descripción                                    |
|---------------------------|------|------------------------------------------------|
| Delimitador de inicio     | 1    | 0x7E                                           |
| Tamaño                    | 2-3  | Bit mas significativo, bit menos significativo |
| **Datos de la Trama**     | 4-n  | De la estructura de la API.                    |
| Checksum                  | n+1  | 1 byte                                         |


Los **datos de la trama** que empiezan en el byte 4 en adelante, estan representados por la siguiente tabla:

| Campos de la trama         | Byte           | Descripción                                 |
|----------------------------|----------------|---------------------------------------------|
| Tipo de trama              | 4              | 0x10                                        |
| ID de la trama             | 5              | ID de la trama (Inicia siempre en 1)        |
| Dirección de destino(MSB)  | 6-13(8)        | Dirección de destino de 64 bits             |
| Dirección de destino(LSB)  | 14-15          | Dirección de destino de 16 bits (MSB y LSB) |
| Broadcast Radius           | 16             | Especifica el maximo numero de saltos de una transmision broadcast. Si se establece a **0**, los saltos seran los maximos     |
| Opciones                   | 14             | Opciones                                    |
| Datos RF                   | 15-n           | Payload (mensaje a enviar)                  |

Las **opciones**, segun la documentacion, son las siguientes:

| Valor      | Descripción                                              |
|------------|----------------------------------------------------------|
| 0x01       | Inhabilita reintentos y reparar rutas                    |
| 0x20       | Habilita el cifrado APS (si EE = 1)                      |
| 0x40       | Use el tiempo de espera de transmisión extendido         |


### Tamaño de la trama
Para calcular el Tamaño de la trama se excluyen el Delimitador y el Checksum. De esta manera se obtiene el length y se deben partir en *bits mas significativos(MSB) y bits menos significativos(LSB)*

### Checksum
**Calcular:** Para calcular el checksum no se incluyen ni el delimitador de inicio ni el tamaño de la trama. Se suman el resto de bytes y se conservan los 8 bits mas bajos del resultado. Luego restaremos el resultado de 0xFF y este dara el resultado del checksum.

**Verificar:** Sumar todos los Bytes(incluido el checksum, pero no el delimitador ni el tamaño). Si el checksum es correcto, debe dar igual a 0xFF.

### Ejemplo de transmision y recepcion de tramas.

Miremos la siguiente trama: 7E 00 12 10 01 AA BB CC DD EE FF 11 22 FF FE 00 00 48 6F 6C 61 3F.

| Byte(s)                  | Descripción                          |
|--------------------------|--------------------------------------|
| 7E                       | Delimitador de inicio                |
| 00 12                    | Tamaño de la trama                   |
| 10                       | Tipo de la trama (modo AP)           |
| 01                       | ID de la trama (Inicia siempre en 1) |
| AA BB CC DD EE FF 11 22  | Dirección de destino                 |
| FF FE                    | Dirección de destino 16 BITS         |
| 00                       | Broadcast radius                     |
| 00                       | Opciones                             |
| 48 6F 6C 61              | Payload (Hola)                       |
| 3F                       | Checksum                             |