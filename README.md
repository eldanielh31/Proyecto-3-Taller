# Arduino Mega DTMF Receiver

Módulo digital para el proyecto de sistema TDM/DTMF.  
Este módulo recibe una señal de audio por medio de un micrófono, detecta dos frecuencias DTMF mediante el algoritmo de Goertzel y enciende el LED correspondiente en una matriz de 3 × 3.

## 1. Objetivo del script

El script del Arduino Mega debe cumplir las siguientes funciones:

1. Leer una señal de audio desde un módulo de micrófono conectado a `A0`.
2. Muestrear la señal a 8 kHz.
3. Detectar las frecuencias DTMF usadas por los botones del 1 al 9.
4. Identificar qué botón fue presionado según la combinación de dos frecuencias.
5. Encender el LED correspondiente en una matriz de 9 LEDs.
6. Permitir pruebas de calibración mediante el Monitor Serial.

---

## 2. Hardware utilizado

### Componentes principales

| Cantidad | Componente |
|---:|---|
| 1 | Arduino Mega 2560 |
| 1 | Módulo micrófono FC-04 Voice Sound Detection Sensor Module |
| 9 | LEDs |
| 9 | Resistencias de 220 Ω |
| 1 | Protoboard o placa perforada |
| Varios | Jumpers macho-macho / macho-hembra |
| 1 | Batería de 9 V o fuente de alimentación para Arduino |
| 1 | Cable USB para cargar el código |

---

## 3. Conexiones del micrófono FC-04

El módulo FC-04 normalmente puede tener salidas analógicas y/o digitales.  
Para este proyecto se debe usar la salida analógica.

| FC-04 | Arduino Mega |
|---|---|
| VCC / + | 5V |
| GND / - | GND |
| AO / AOUT | A0 |
| DO / OUT digital | No se usa |

### Nota importante

El pin `DO` o salida digital no sirve para detectar frecuencias DTMF.  
Ese pin solo indica si hay sonido o no.

Para el algoritmo Goertzel se necesita la señal analógica del micrófono, por eso debe usarse:

```text
AO / AOUT → A0
