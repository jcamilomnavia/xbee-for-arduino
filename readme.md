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
Esta libreria esta hecha para trabajar con Arduino Mega, o cualquier arduino que tenga **Serial** y **Serial3**. 

## Uso

El Xbee debe estar conectado al **Serial** del Arduino. Si es conectado al Serial1 o cualquier otro, esta librería no funcionará.

### Ejemplo en Arduino

Para transmitir tramas entre Arduinos Mega:
```C
#include <Xbee.h>
Xbee myXbee = Xbee();

void setup(){
  Serial.begin(9600);
  Serial3.begin();
  myXbee.setPayload("Hola mundo");
  myXbee.setDestinationAddress(0xAABBCCDDEEFF1122);
  // myXbee.setBroadcastRadius(0x02);
  // myXbee.setOption(0x20);
  // ... otras opciones
  myXbee.send();
}

void loop(){
  
}
```

Para recibir tramas:
```C
#include <Xbee.h>
Xbee myXbee = Xbee();

void setup(){
  Serial.begin(9600);
  Serial3.begin();
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
| Opciones                   | 17             | Opciones                                    |
| Datos RF                   | 18-n           | Payload (mensaje a enviar)                  |

Las **opciones**, segun la documentacion, son las siguientes:

| Valor      | Descripción                                              |
|------------|----------------------------------------------------------|
| 0x01       | Inhabilita reintentos y reparar rutas                    |
| 0x20       | Habilita el cifrado APS (si EE = 1)                      |
| 0x40       | Use el tiempo de espera de transmisión extendido         |


### Tamaño de la trama
Para calcular el Tamaño de la trama se excluyen el Delimitador y el Checksum ( Y claramente los Bytes del tamaño, pues apenas se calcularas). Luego sumaremos cuantos Bytes tenemos. Es decir, si tenemos 0xFF 0xFF 0xFF tenemos 3 bytes en total. De esta manera se obtiene el length y se deben partir en *8 bits mas significativos(MSB) y 8 bits menos significativos(LSB)* para ser enviados en la trama.

Sabemos bien que en la trama hay un tamaño de Bytes fijo que es 14. Conformados por: 
- 1 Byte de Tipo de trama
- 1 Byte de ID de Trama
- 8 Bytes de Direccion destino
- 2 Bytes de Direccion de 16 bits
- 1 Byte de Broadcast Radius
- 1 Bytes de Opciones

El resto del tamaño de la trama dependera de la longitud del Payload.

### Checksum

El checksum es una suma de verificación (o suma de chequeo), es una función de redundancia que tiene como propósito principal detectar cambios accidentales en una secuencia de datos para proteger la integridad de estos mismos, verificando que no haya discrepancias entre los valores obtenidos al hacer una comprobación inicial y otra final tras la transmisión. 

**Calcular:** Para calcular el checksum de una trama se hace lo siguiente: se suman todos sus elementos (Bytes) excepto el delimitador de incio y el tamaño de la trama. Luego, con los 8 bits mas bajos del resultado, se lo restamos a 0xFF y ese seria el checksum de la trama.

**Verificar:** Para verificar la trama, sumaremos todos los Bytes de la trama (menos el delimitador y el tamaño de la trama, pero incluyendo el checksum) y el resultado debe dar 0xFF.

### Ejemplo de transmision y recepcion de tramas.

#### Transmisión

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

Para obtener el anterior tamaño de la trama sumaremos la cantidad de Bytes fijos que hay (14) y simplemente le sumaremos 4(tamaño del payload) lo cual nos dará 18, este valor en hexadecimal es representado como 0x12.
Luego obtendremos su MSB(0x00) y su LSB(0x12).

Para obtener el anterior checksum sumamos desde el tipo de trama en adelante hasta el checksum y :
0x10 + 0x01 + 0xAA + 0xBB+ 0xCC + 0xDD + 0xEE + 0xFF + 0x11 + 0x22 + 0xFF + 0xFE + 0x00 + 0x00 + 0x48 + 0x6F + 0x6C + 0x61 = 0x8C0
Ahora restamos 0x8C0 (sus 8 bits mas bajos, 0xC0) de 0xFF, con esto obtenemos que:
0xFF - 0xC0 = 0x3F

#### Recepción
Recibiremos (por serial en este caso) la anterior trama:  7E 00 12 10 01 AA BB CC DD EE FF 11 22 FF FE 00 00 48 6F 6C 61 3F.

Sabremos que es una nueva trama por el delimitador de inicio: 0x7E

A partir de aqui, hasta el Byte de opciones, son tamaños y posiciones fijas, por lo que podremos decodificar facilmente la información de la trama (Las posiciones de cada Byte estan descritos en la tabla 2 y 3 de este documento).

Para el tamaño, obtendremos sus 2 Bytes y se sumarán, pues el primer Byte(MSB) y el segundo(LSB) formarán una variable de 16 bits. En este caso 0x00 + 0x12 = 0x12 = 18 en decimal.

Desde el tipo de trama hasta el byte de opciones no requiere mayor explicación. Solo hay que tener en cuenta las posiciones fijas de cada Byte. Para esta librería se implementa un switch case.

Sabemos que el payload empieza en el Byte 18. Desde aqui hasta el penultimo Byte de la trama o ultima posicion del tamaño anteriormente obtenido(recordar que el tamaño solo hace referencia desde el Byte 4 hasta el final del payload, ver tabla 2 para mayor comprensión), sera todo nuestro Payload. En este caso:  48 6F 6C 61 = "Hola".

Verificaremos la trama recibida: 
0x10 + 0x01 + 0xAA + 0xBB+ 0xCC + 0xDD + 0xEE + 0xFF + 0x11 + 0x22 + 0xFF + 0xFE + 0x00 + 0x00 + 0x48 + 0x6F + 0x6C + 0x61 + **0x3F** = 0xFF.

La suma es 0xFF por lo que la trama fue bien recibida.

Finalmente imprimiremos la información decodificada en serial, todos los valores seran impresos en formato Hexadecimal excepto el payload y el tamaño de la trama.
- Para imprimir en decimal usaremos ```Serial.print(value, HEX)```
- Para imprimir en formato ASCII podemos asignar el valor leído a un ```char``` y luego imprimirlo con ```Serial.print(var)```. Esto para el payload.