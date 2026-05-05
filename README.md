# Arduino Mega DTMF Receiver

Módulo digital para el proyecto de sistema TDM/DTMF.

Este módulo recibe una señal de audio mediante un módulo de micrófono, detecta dos frecuencias DTMF usando el algoritmo de Goertzel y enciende el LED correspondiente en una matriz de 3 × 3.

---

## 1. Objetivo del script

El script del Arduino Mega debe cumplir lo siguiente:

1. Leer la señal de audio desde un módulo de micrófono conectado a `A0`.
2. Muestrear la señal a `8 kHz`.
3. Detectar las frecuencias DTMF usadas por los botones del 1 al 9.
4. Identificar el botón presionado según la combinación de dos frecuencias.
5. Encender el LED correspondiente en una matriz de 9 LEDs.
6. Permitir pruebas de calibración mediante el Monitor Serial.

---

## 2. Hardware utilizado

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

El módulo FC-04 puede tener salidas analógicas y/o digitales. Para este proyecto se debe usar la salida analógica.

| FC-04 | Arduino Mega |
|---|---|
| VCC / + | 5V |
| GND / - | GND |
| AO / AOUT | A0 |
| DO / OUT digital | No se usa |

### Nota importante

El pin `DO` o salida digital no sirve para detectar frecuencias DTMF. Ese pin solo indica si hay sonido o no.

Para el algoritmo Goertzel se necesita la señal analógica del micrófono:

```text
AO / AOUT → A0
```

Si el módulo disponible solo tiene salida digital `OUT`, no es adecuado para este script.

---

## 4. Conexiones de los LEDs

Cada LED debe tener su propia resistencia de 220 Ω.

| LED | Pin Arduino Mega |
|---|---:|
| LED 1 | 22 |
| LED 2 | 23 |
| LED 3 | 24 |
| LED 4 | 25 |
| LED 5 | 26 |
| LED 6 | 27 |
| LED 7 | 28 |
| LED 8 | 29 |
| LED 9 | 30 |

Conexión recomendada por LED:

```text
Pin Arduino → Resistencia 220 Ω → Ánodo LED
Cátodo LED → GND
```

La matriz debe verse así:

```text
LED1  LED2  LED3
LED4  LED5  LED6
LED7  LED8  LED9
```

Esta distribución debe coincidir con el teclado físico del proyecto.

---

## 5. Frecuencias DTMF utilizadas

El proyecto usa 6 frecuencias:

| Tipo | Frecuencias |
|---|---|
| Frecuencias bajas / filas | 697 Hz, 770 Hz, 852 Hz |
| Frecuencias altas / columnas | 1209 Hz, 1336 Hz, 1477 Hz |

Mapeo de botones:

| Botón | Frecuencia baja | Frecuencia alta | LED |
|---|---:|---:|---|
| 1 | 697 Hz | 1209 Hz | LED 1 |
| 2 | 697 Hz | 1336 Hz | LED 2 |
| 3 | 697 Hz | 1477 Hz | LED 3 |
| 4 | 770 Hz | 1209 Hz | LED 4 |
| 5 | 770 Hz | 1336 Hz | LED 5 |
| 6 | 770 Hz | 1477 Hz | LED 6 |
| 7 | 852 Hz | 1209 Hz | LED 7 |
| 8 | 852 Hz | 1336 Hz | LED 8 |
| 9 | 852 Hz | 1477 Hz | LED 9 |

---

## 6. Justificación de muestreo

La frecuencia más alta utilizada es:

```text
1477 Hz
```

Por el teorema de Nyquist:

```text
fs > 2 × 1477
fs > 2954 Hz
```

El script usa:

```text
fs = 8000 Hz
```

Esto permite detectar correctamente las frecuencias DTMF usadas en el proyecto.

---

## 7. Parámetros principales del script

Constantes principales:

```cpp
const byte AUDIO_PIN = A0;

const byte ledPins[9] = {
  22, 23, 24,
  25, 26, 27,
  28, 29, 30
};

const float FS = 8000.0;
const unsigned int SAMPLE_PERIOD_US = 125;
const int N = 205;
```

Parámetros ajustables:

```cpp
float MIN_POWER = 100000.0;
float RATIO = 1.4;
```

| Parámetro | Función |
|---|---|
| `MIN_POWER` | Potencia mínima para considerar que una frecuencia está presente |
| `RATIO` | Diferencia mínima entre la frecuencia dominante y las demás |
| `REQUIRED_STABLE` | Cantidad de detecciones iguales seguidas antes de encender un LED |

---

## 8. Comandos del Monitor Serial

El Monitor Serial debe abrirse a:

```text
115200 baudios
```

Comandos disponibles:

| Comando | Función |
|---|---|
| `1` a `9` | Prueba manual del LED correspondiente |
| `D` | Activa o desactiva la impresión de potencias Goertzel |
| `R` | Activa o desactiva la impresión de lectura cruda del micrófono |

---

## 9. Prueba inicial de LEDs

Antes de probar el micrófono, validar que los LEDs estén bien conectados.

### Procedimiento

1. Cargar el código en el Arduino Mega.
2. Abrir el Monitor Serial a `115200`.
3. Escribir un número del `1` al `9`.
4. Verificar que se encienda el LED correspondiente.

### Resultado esperado

| Entrada Serial | Resultado |
|---|---|
| `1` | Enciende LED 1 |
| `5` | Enciende LED 5 |
| `9` | Enciende LED 9 |

Si un LED no enciende, revisar:

- Polaridad del LED.
- Resistencia.
- Pin asignado.
- Conexión a GND.
- Continuidad del cableado.

---

## 10. Prueba del micrófono en modo RAW

El modo RAW sirve para verificar si el FC-04 entrega señal analógica válida.

### Procedimiento

1. Cargar el código.
2. Abrir el Monitor Serial a `115200`.
3. Escribir:

```text
R
```

4. Observar la salida del Monitor Serial.

Formato esperado:

```text
RAW AVG=512 | MIN=500 | MAX=525 | P2P=25
```

### Interpretación

| Resultado | Interpretación |
|---|---|
| `P2P` entre 1 y 10 en silencio | Normal |
| `P2P` aumenta con sonido | El micrófono funciona |
| `MIN=0` y `MAX=1023` | Señal saturada |
| Siempre cerca de `0` | Mala conexión o salida digital |
| Siempre cerca de `1023` | Mala conexión o saturación |
| No cambia con sonido | Micrófono mal conectado o salida incorrecta |

### Resultado deseado

En silencio:

```text
RAW AVG=500 a 520
P2P=1 a 20
```

Con sonido o tono:

```text
P2P mayor que en silencio
```

Idealmente, con tono fuerte pero sin saturar:

```text
P2P=50 a 300
```

La señal no debe pegarse constantemente en `0` ni en `1023`.

---

## 11. Prueba de detección Goertzel

Después de verificar que el micrófono funciona, probar la detección de frecuencias.

### Procedimiento

1. Apagar el modo RAW si está activo escribiendo:

```text
R
```

2. Activar el modo de potencias escribiendo:

```text
D
```

3. Reproducir o generar tonos DTMF.
4. Observar las potencias impresas.

Formato esperado:

```text
697=12000 | 770=8000 | 852=9000 | 1209=15000 | 1336=7000 | 1477=6000
```

---

## 12. Pruebas mínimas recomendadas

Antes de probar los 9 botones, probar los botones extremos y el centro.

| Botón | Frecuencias esperadas |
|---|---|
| 1 | 697 Hz + 1209 Hz |
| 5 | 770 Hz + 1336 Hz |
| 9 | 852 Hz + 1477 Hz |

### Botón 1

Deben subir principalmente:

```text
697 y 1209
```

### Botón 5

Deben subir principalmente:

```text
770 y 1336
```

### Botón 9

Deben subir principalmente:

```text
852 y 1477
```

Si esos tres funcionan, continuar con los demás botones.

---

## 13. Datos que se deben copiar para calibrar

Para ajustar el script correctamente, copiar del Monitor Serial los siguientes bloques:

```text
SILENCIO:
[10 líneas en modo D]

BOTON 1:
[10 líneas en modo D]

BOTON 5:
[10 líneas en modo D]

BOTON 9:
[10 líneas en modo D]
```

Ejemplo de formato:

```text
SILENCIO:
697=... | 770=... | 852=... | 1209=... | 1336=... | 1477=...
697=... | 770=... | 852=... | 1209=... | 1336=... | 1477=...

BOTON 1:
697=... | 770=... | 852=... | 1209=... | 1336=... | 1477=...

BOTON 5:
697=... | 770=... | 852=... | 1209=... | 1336=... | 1477=...

BOTON 9:
697=... | 770=... | 852=... | 1209=... | 1336=... | 1477=...
```

Con esos datos se ajustan:

```cpp
float MIN_POWER = ...;
float RATIO = ...;
```

---

## 14. Ajuste de `MIN_POWER`

`MIN_POWER` evita que el ruido sea interpretado como una frecuencia válida.

### Si no detecta nada

Bajar `MIN_POWER`:

```cpp
float MIN_POWER = 50000.0;
```

### Si detecta botones sin sonido

Subir `MIN_POWER`:

```cpp
float MIN_POWER = 300000.0;
```

### Regla práctica

```text
MIN_POWER debe ser mayor que el ruido en silencio,
pero menor que la potencia de los tonos reales.
```

Ejemplo:

```text
Ruido máximo en silencio: 20000
Tono real mínimo: 200000
```

Valor recomendado:

```cpp
float MIN_POWER = 80000.0;
```

---

## 15. Ajuste de `RATIO`

`RATIO` exige que la frecuencia detectada sea claramente mayor que las demás.

### Si detecta botones incorrectos

Subir `RATIO`:

```cpp
float RATIO = 1.8;
```

O:

```cpp
float RATIO = 2.0;
```

### Si detecta muy poco, pero las frecuencias correctas sí suben

Bajar `RATIO`:

```cpp
float RATIO = 1.3;
```

### Valores recomendados iniciales

```cpp
float MIN_POWER = 100000.0;
float RATIO = 1.4;
```

---

## 16. Ajuste físico del micrófono FC-04

Antes de cambiar mucho el código, ajustar físicamente la señal.

| Elemento | Recomendación |
|---|---|
| Distancia micrófono-parlante | 5 cm a 15 cm |
| Volumen del emisor | Medio |
| Ruido ambiente | Bajo |
| Potenciómetro del FC-04 | Ajustar lentamente |
| Entrada usada | AO / AOUT, no DO |
| Saturación | Evitar lecturas 0 o 1023 constantes |

El potenciómetro del FC-04 puede afectar principalmente el umbral de la salida digital. Si se usa `AO`, el cambio puede ser limitado, pero conviene probarlo.

---

## 17. Problemas comunes

### Problema: los LEDs no encienden

Revisar:

```text
- Pines 22 a 30.
- Polaridad de LEDs.
- Resistencias.
- GND común.
- Prueba serial 1-9.
```

### Problema: el micrófono no cambia

Revisar:

```text
- VCC a 5V.
- GND a GND.
- AO a A0.
- No usar DO.
- Probar modo RAW con comando R.
```

### Problema: la señal se satura

Síntoma:

```text
MIN=0 | MAX=1023 | P2P=1023
```

Soluciones:

```text
- Bajar volumen.
- Alejar micrófono.
- Revisar que no se esté usando una etapa amplificada excesiva.
- Verificar que la señal no supere 5V.
```

### Problema: detecta botones sin sonido

Soluciones:

```text
- Subir MIN_POWER.
- Subir RATIO.
- Reducir ruido ambiente.
- Apagar modo D para presentación final.
```

### Problema: detecta el botón incorrecto

Soluciones:

```text
- Subir RATIO.
- Revisar que el emisor realmente produzca las frecuencias correctas.
- Bajar ruido externo.
- Ajustar distancia micrófono-parlante.
```

### Problema: no detecta tonos reales

Soluciones:

```text
- Bajar MIN_POWER.
- Revisar modo D para ver si las frecuencias correctas suben.
- Aumentar volumen moderadamente.
- Acercar micrófono.
```

---

## 18. Qué debe hacerse para dejar versión final

Antes de considerar el script finalizado, completar este proceso:

### Paso 1: validar LEDs

```text
Todos los LEDs del 1 al 9 deben encender correctamente usando el Monitor Serial.
```

### Paso 2: validar micrófono

```text
El modo RAW debe mostrar variación con sonido.
La lectura no debe saturarse constantemente en 0 o 1023.
```

### Paso 3: validar botones clave

Probar:

```text
Botón 1
Botón 5
Botón 9
```

Estos deben detectar correctamente sus LEDs.

### Paso 4: validar los 9 botones

Probar todos:

```text
1, 2, 3
4, 5, 6
7, 8, 9
```

Cada botón debe encender solamente su LED correspondiente.

### Paso 5: ajustar umbrales finales

Dejar fijos:

```cpp
float MIN_POWER = valor_final;
float RATIO = valor_final;
```

### Paso 6: desactivar debug para presentación

Dejar al inicio:

```cpp
bool DEBUG_POWERS = false;
bool DEBUG_RAW = false;
```

### Paso 7: limpiar salida serial

Para la presentación final, dejar solo esta impresión:

```cpp
Serial.print("Boton detectado: ");
Serial.println(detectedButton);
```

También se pueden eliminar las impresiones si el sistema debe funcionar sin computadora.

### Paso 8: fijar hardware

Una vez validado:

```text
- Soldar o fijar conexiones.
- Ocultar cables.
- Asegurar GND común.
- Asegurar el micrófono en posición estable.
- Asegurar que el parlante/emisor no se mueva.
```

### Paso 9: prueba con alimentación final

El sistema debe probarse con la alimentación final del proyecto, no solo por USB.

### Paso 10: documentar resultados

Guardar evidencia:

```text
- Foto del Arduino y LEDs.
- Foto del micrófono.
- Foto de la matriz 3x3.
- Video de los 9 botones funcionando.
- Tabla de pruebas botón → LED.
- Valores finales de MIN_POWER y RATIO.
```

---

## 19. Checklist final

| Tarea | Estado |
|---|---|
| Arduino Mega carga el código sin errores | ☐ |
| FC-04 conectado por AO a A0 | ☐ |
| DO no se usa | ☐ |
| LEDs conectados en pines 22-30 | ☐ |
| Cada LED tiene resistencia de 220 Ω | ☐ |
| Prueba serial de LEDs completada | ☐ |
| Modo RAW probado | ☐ |
| Micrófono responde al sonido | ☐ |
| Señal no satura constantemente | ☐ |
| Modo Goertzel probado | ☐ |
| Botón 1 detectado | ☐ |
| Botón 5 detectado | ☐ |
| Botón 9 detectado | ☐ |
| Los 9 botones detectados | ☐ |
| Umbrales calibrados | ☐ |
| Debug apagado para versión final | ☐ |
| Sistema probado con alimentación final | ☐ |
| Módulo integrado en maqueta | ☐ |
| Video de funcionamiento grabado | ☐ |
| Explicación técnica lista | ☐ |

---

## 20. Versión final esperada

La versión final del módulo Arduino debe comportarse así:

```text
1. El sistema se enciende.
2. El micrófono recibe la señal de audio DTMF.
3. El Arduino Mega procesa la señal en A0.
4. El algoritmo Goertzel identifica una frecuencia baja y una alta.
5. El código traduce la combinación al botón correspondiente.
6. Se enciende el LED en la misma posición del botón presionado.
7. Al dejar de recibir señal válida, el LED se apaga.
```

Resultado final esperado:

```text
Botón 1 → LED 1
Botón 2 → LED 2
Botón 3 → LED 3
Botón 4 → LED 4
Botón 5 → LED 5
Botón 6 → LED 6
Botón 7 → LED 7
Botón 8 → LED 8
Botón 9 → LED 9
```

---

## 21. Texto técnico para el documento

La etapa digital del proyecto fue implementada mediante un Arduino Mega 2560. La señal de audio proveniente del módulo de micrófono FC-04 se conecta al pin analógico A0, desde donde se realiza el muestreo de la señal a una frecuencia de 8 kHz. Esta frecuencia de muestreo cumple el criterio de Nyquist, ya que la frecuencia máxima utilizada por el sistema DTMF es de 1477 Hz.

Para detectar las frecuencias presentes se utiliza el algoritmo de Goertzel, el cual permite evaluar frecuencias específicas sin necesidad de aplicar una FFT completa. El sistema analiza seis frecuencias DTMF: 697 Hz, 770 Hz, 852 Hz, 1209 Hz, 1336 Hz y 1477 Hz. Cuando se detecta una frecuencia del grupo bajo y una frecuencia del grupo alto, el Arduino determina el botón correspondiente y enciende el LED equivalente en una matriz de 3 × 3.

La matriz de LEDs conserva la misma distribución espacial del teclado de entrada, por lo que el LED encendido coincide con la posición del botón presionado.
