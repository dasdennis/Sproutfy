#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
// #include "calibration.h" // Inclui o script auxiliar de calibração

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme;
RTC_DS3231 rtc;

const int soilMoisturePins[] = {A2, A3, A4, A5};
const int pumpRelayPins[] = {1, 2, 3, 4};
const int numSensors = sizeof(soilMoisturePins) / sizeof(soilMoisturePins[0]);

// Configurando exibição e controle de tempo
unsigned long previousMillis = 0;
const long displayInterval = 3000;
int currentSensorIndex = 0;

void setup() {
  Serial.begin(9600);

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED nao encontrado!"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Inicializa o sensor BME280
  if (!bme.begin(0x76)) {
    Serial.println(F("BME280 nao encontrado!"));
    while (true);
  }

  // Inicializa o RTC
  if (!rtc.begin()) {
    Serial.println(F("RTC não encontrado!"));
    while (true);
  }
  if (rtc.lostPower()){
    Serial.println(F("RTC low power, setting default time!"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  for (int i = 0; i < numSensors; i++) {
    pinMode(soilMoisturePins[i], INPUT);
    pinMode(pumpRelayPins[i], OUTPUT);
    digitalWrite(pumpRelayPins[i], HIGH);
  }

// Função para ler umidade do solo
int readSoilMoisture(int pin) {
  int rawValue = analogRead(pin);
  int moisturePercent = map(rawValue, 0, 1023, 0, 100);
  return moisturePercent;
}

void controlPump(int moisture, int relayPin) {
  if (moisture < 30) {
    digitalWrite(relayPin, LOW); // LIGA a bomba d'agua
  } else if (moisture > 60) {
    digitalWrite(relayPin, HIGH); // DESLIGA a bomba d'agua
  }
}

// Função para exibir os dados dos sensores no display OLED
void displaySensorData(int sensorIndex, int moisture) {
  display.clearDisplay();

  DateTime now = rtc.now();
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();

  display.setCursor(0, 0);
  display.print(F("Sensor: "));
  display.print(sensorIndex + 1);
  display.print(F("/"));
  display.print(numSensors);
  display.setCursor(0, 10);
  display.print(F("Soil Moisture: "));
  display.print(moisture);
  display.print(F("%"));

  display.setCursor(0, 20);
  display.print(F("Temp: "));
  display.print(temperature);
  display.print(F(" C"));

  display.setCursor(0, 30);
  display.print(F("Humidity: "));
  display.print(humidity);
  display.print(F(" %"));

  display.setCursor(0, 40);
  display.print(now.year(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.print(now.day(), DEC);
  display.print(" ");
  display.print(now.hour(), DEC);
  display.print(':');
  display.print(now.minute(), DEC);
  display.print(':');
  display.print(now.second(), DEC);

  display.display();
}

void loop() {
  unsigned long currentMillis = millis();

  // Valida se o intervalo de exibição passou
  if (currentMillis - previousMillis >= displayInterval) {
    previousMillis = currentMillis;

    // Realiza leitura, controle da bomba e exibição dos dados
    int moisture = readSoilMoisture(soilMoisturePins[currentSensorIndex]);
    controlPump(moisture, pumpRelayPins[currentSensorIndex]);
    displaySensorData(currentSensorIndex, moisture);

    // Avança para o próximo sensor
    currentSensorIndex = (currentSensorIndex + 1) % numSensors;
  }
}



