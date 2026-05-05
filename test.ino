#include <Arduino.h>
#include <math.h>

// =====================================================
// Proyecto: Receptor DTMF con Arduino Mega
// Sensor: FC-04 Voice Sound Detection Sensor Module
// Entrada recomendada: AO / AOUT del FC-04 hacia A0
// Salida: Matriz de 9 LEDs en pines 22-30
// Detección: Goertzel
// =====================================================

// -------------------------
// Pines
// -------------------------
const byte AUDIO_PIN = A0;

// Pines para LEDs 1 al 9
const byte ledPins[9] = {
  22, 23, 24,
  25, 26, 27,
  28, 29, 30
};

// -------------------------
// Frecuencias DTMF
// -------------------------
// Filas: 697, 770, 852 Hz
// Columnas: 1209, 1336, 1477 Hz
const float freqs[6] = {
  697.0,
  770.0,
  852.0,
  1209.0,
  1336.0,
  1477.0
};

// -------------------------
// Muestreo
// -------------------------
const float FS = 8000.0;                    // 8 kHz
const unsigned int SAMPLE_PERIOD_US = 125;  // 1 / 8000 = 125 us
const int N = 205;                          // muestras por bloque

int samples[N];
float coeffs[6];

// -------------------------
// Parámetros ajustables
// -------------------------
// Para FC-04 se empieza más bajo porque la señal puede variar bastante.
float MIN_POWER = 100000.0;
float RATIO = 1.4;

// Detecciones iguales consecutivas necesarias
const int REQUIRED_STABLE = 2;

int lastButton = 0;
int stableCount = 0;
int lastShownButton = 0;

// Modos de depuración
bool DEBUG_POWERS = false;
bool DEBUG_RAW = false;

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 9; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Precalcular coeficientes Goertzel
  for (int i = 0; i < 6; i++) {
    float k = round((N * freqs[i]) / FS);
    float omega = (2.0 * PI * k) / N;
    coeffs[i] = 2.0 * cos(omega);
  }

  Serial.println("========================================");
  Serial.println("Receptor DTMF listo");
  Serial.println("Arduino Mega + FC-04 + LEDs 3x3");
  Serial.println("Conectar AO/AOUT del FC-04 a A0");
  Serial.println("El pin DO/OUT digital no se usa para DTMF");
  Serial.println("========================================");
  Serial.println("Comandos:");
  Serial.println("1-9 = probar LED");
  Serial.println("D = activar/desactivar potencias Goertzel");
  Serial.println("R = activar/desactivar lectura cruda");
  Serial.println("========================================");
}

// =====================================================
// LOOP PRINCIPAL
// =====================================================
void loop() {
  checkSerialCommands();

  captureSamples();

  if (DEBUG_RAW) {
    printRawStats();
    delay(150);
    return;
  }

  int detectedButton = detectButton();

  if (detectedButton > 0) {
    if (detectedButton == lastButton) {
      stableCount++;
    } else {
      lastButton = detectedButton;
      stableCount = 1;
    }

    if (stableCount >= REQUIRED_STABLE) {
      showButton(detectedButton);

      if (detectedButton != lastShownButton) {
        Serial.print("Boton detectado: ");
        Serial.println(detectedButton);
        lastShownButton = detectedButton;
      }
    }
  } else {
    lastButton = 0;
    stableCount = 0;
    lastShownButton = 0;
    clearLeds();
  }
}

// =====================================================
// Comandos por Monitor Serial
// =====================================================
void checkSerialCommands() {
  if (!Serial.available()) {
    return;
  }

  char c = Serial.read();

  if (c >= '1' && c <= '9') {
    int button = c - '0';

    showButton(button);

    Serial.print("Prueba LED ");
    Serial.println(button);

    delay(500);
    clearLeds();
  }

  if (c == 'd' || c == 'D') {
    DEBUG_POWERS = !DEBUG_POWERS;

    Serial.print("DEBUG_POWERS = ");
    Serial.println(DEBUG_POWERS ? "ON" : "OFF");
  }

  if (c == 'r' || c == 'R') {
    DEBUG_RAW = !DEBUG_RAW;

    Serial.print("DEBUG_RAW = ");
    Serial.println(DEBUG_RAW ? "ON" : "OFF");
  }
}

// =====================================================
// Captura de muestras desde A0
// =====================================================
void captureSamples() {
  unsigned long nextSampleTime = micros();
  long sum = 0;

  for (int i = 0; i < N; i++) {
    while ((long)(micros() - nextSampleTime) < 0) {
      // Espera al siguiente instante de muestreo
    }

    int raw = analogRead(AUDIO_PIN);

    samples[i] = raw;
    sum += raw;

    nextSampleTime += SAMPLE_PERIOD_US;
  }

  // Quitar componente DC
  int mean = sum / N;

  for (int i = 0; i < N; i++) {
    samples[i] = samples[i] - mean;
  }
}

// =====================================================
// Detectar botón DTMF
// =====================================================
int detectButton() {
  float powers[6];

  for (int i = 0; i < 6; i++) {
    powers[i] = goertzelPower(i);
  }

  if (DEBUG_POWERS) {
    printPowers(powers);
  }

  int lowIndex = maxIndex(powers, 0, 2);
  int highIndex = maxIndex(powers, 3, 5);

  float lowMax = powers[lowIndex];
  float highMax = powers[highIndex];

  float lowSecond = secondMaxValue(powers, 0, 2, lowIndex);
  float highSecond = secondMaxValue(powers, 3, 5, highIndex);

  bool lowValid = lowMax > MIN_POWER && lowMax > (lowSecond * RATIO);
  bool highValid = highMax > MIN_POWER && highMax > (highSecond * RATIO);

  if (!lowValid || !highValid) {
    return 0;
  }

  int row = lowIndex;       // 0, 1, 2
  int col = highIndex - 3;  // 0, 1, 2

  int button = row * 3 + col + 1;

  return button;
}

// =====================================================
// Algoritmo Goertzel
// =====================================================
float goertzelPower(int freqIndex) {
  float s_prev = 0.0;
  float s_prev2 = 0.0;
  float coeff = coeffs[freqIndex];

  for (int i = 0; i < N; i++) {
    float s = samples[i] + coeff * s_prev - s_prev2;
    s_prev2 = s_prev;
    s_prev = s;
  }

  float power = s_prev2 * s_prev2 + s_prev * s_prev - coeff * s_prev * s_prev2;

  return power;
}

// =====================================================
// Utilidades de cálculo
// =====================================================
int maxIndex(float arr[], int start, int end) {
  int idx = start;
  float maxVal = arr[start];

  for (int i = start + 1; i <= end; i++) {
    if (arr[i] > maxVal) {
      maxVal = arr[i];
      idx = i;
    }
  }

  return idx;
}

float secondMaxValue(float arr[], int start, int end, int excludeIndex) {
  float second = 0.0;

  for (int i = start; i <= end; i++) {
    if (i == excludeIndex) {
      continue;
    }

    if (arr[i] > second) {
      second = arr[i];
    }
  }

  return second;
}

// =====================================================
// LEDs
// =====================================================
void showButton(int button) {
  clearLeds();

  if (button >= 1 && button <= 9) {
    digitalWrite(ledPins[button - 1], HIGH);
  }
}

void clearLeds() {
  for (int i = 0; i < 9; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

// =====================================================
// Debug de potencias Goertzel
// =====================================================
void printPowers(float powers[]) {
  Serial.print("697=");
  Serial.print(powers[0], 0);

  Serial.print(" | 770=");
  Serial.print(powers[1], 0);

  Serial.print(" | 852=");
  Serial.print(powers[2], 0);

  Serial.print(" | 1209=");
  Serial.print(powers[3], 0);

  Serial.print(" | 1336=");
  Serial.print(powers[4], 0);

  Serial.print(" | 1477=");
  Serial.println(powers[5], 0);
}

// =====================================================
// Debug de señal cruda
// =====================================================
void printRawStats() {
  int minVal = 1023;
  int maxVal = 0;
  long sum = 0;

  for (int i = 0; i < N; i++) {
    int value = samples[i];

    // samples[] ya está sin DC, así que se reconstruye solo para estadística aproximada.
    // Para ver datos crudos reales, se vuelve a leer A0 aquí.
    int raw = analogRead(AUDIO_PIN);

    if (raw < minVal) minVal = raw;
    if (raw > maxVal) maxVal = raw;
    sum += raw;
  }

  int avg = sum / N;
  int p2p = maxVal - minVal;

  Serial.print("RAW AVG=");
  Serial.print(avg);

  Serial.print(" | MIN=");
  Serial.print(minVal);

  Serial.print(" | MAX=");
  Serial.print(maxVal);

  Serial.print(" | P2P=");
  Serial.println(p2p);
}
