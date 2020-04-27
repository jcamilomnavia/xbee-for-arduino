# Xbee para Arduino

Esta libreria pretende transmitir tramas api con el modulo Xbee S1, donde el usuario simplemente ingresa una direccion destino de 64 bits y el mensaje a transmitir. Además podrá configurar opciones adicionales si lo desea.

Para entender mejor la libreria se explicaran las tramas api con direcciones de 64 bits para el modulo Xbee s1.

## Instalación
Arduino incluye un manejador de librerias para una facil instalacion. Del menu Programa/Sketch selecciona ```Incluir libreria->Añadir biblioteca .ZIP``` y selecciona el archivo .zip de esta libreria. 

Una segunda opcion es arrastrar la carpeta de esta libreria manualmente a la carpeta de ```librerias``` de Arduino.

Para usar esta libreria en tu sketch deberás añadir la siguiente linea al principio de tu archivo ```.ino```

```C
#include <Xbee.h>
```

## Configuración 

Para usar esta libreria, tu XBee debe estar configurado en modo API (AP=1). Puedes leer la documentacion que esta mas abajo para ver que tipos de modo hay en Xbee.

## Uso

El Xbee debe estar conectado al **Serial** del Arduino. Si es conectado al Serial1 o cualquier otro, no funcionará.

### Ejemplo

```C
#include "Xbee.h"
Xbee myXbee = Xbee();

void setup(){
  Serial.begin(9600);
  myXbee.setPayload("Hola mundo");
}

void loop(){
  myXbee.setPayload("Hola mundo");
  myXbee.setDestinationAddress(0000000000000000);
  myXbee.send();
}
```

## Documentación de XBee

### Especificaciones de la trama API

El Xbee soporta 3 modos de operación, de los cuales 2 son en modo api. Estos se pueden configurar en la terminal del Xbee en XCTU.
Estas son los modos:

| Comando AP | Descripción                                              |
|------------|----------------------------------------------------------|
| AP = 0     | Modo transparente o AT. Esta es la configuración inicial |
| AP = 1     | Operacion API                                            |
| AP = 2     | Operacion API con caracteres de escape                   |

Las estructura de las tramas API depende del modo que se escoja. En esta libreria solo se implementará la opcion 1, sin caracteres de escape.

### Operación AP 1

La siguiente tabla muestra la estructura de la trama cuando se habilita este modo:

| Campos de la trama    | Byte | Descripción                                    |
|-----------------------|------|------------------------------------------------|
| Delimitador de inicio | 1    | 0x7E                                           |
| Tamaño                | 2-3  | Bit mas significativo, bit menos significativo |
| Datos de la Trama     | 4-n  | De la estructura de la API.                    |
| Checksum              | n+1  | 1 byte                                         |


Los datos de la trama estan representados por la siguiente tabla (Para direcciones de 64 bits)(El payload se envia como un paquete RF):

| Campos de la trama    | Byte           | Descripción                          |
|-----------------------|----------------|--------------------------------------|
| Tipo de trama         | 4              | 0x01                                 |
| ID de la trama        | 5              | ID de la trama (Inicia siempre en 1) |
| Dirección de destino  | 6-13(8)        | Dirección de destino                 |
| Opciones              | 14             | Opciones (0x01 = Desactivar ACK)     |
| Datos RF              | 15-n           | Payload de hasta 100 bytes           |

Miremos la siguiente trama: 7E 00 0A **01 01 50 01 01 48 65 6C 6C 6F** B8.

| Byte(s)        | Descripción                          |
|----------------|--------------------------------------|
| 7E             | Delimitador de inicio                |
| 00 0A          | Tamaño de la trama                   |
| 01             | Tipo de la trama (modo AP)           |
| 01             | ID de la trama (Inicia siempre en 1) |
| 50 01          | Dirección de destino (debe ser de 64 bits, no 16)       |
| 01             | Opciones                             |
| 48 65 6C 6C 6F | Payload                              |
| B8             | Checksum                             |


Para calcular el Tamaño de la trama se excluyen el delimitador y el checksum. De esta manera se obtiene el length y se deben partir en *bits mas significativos(MSB) y bits menos significativos(LSB)*