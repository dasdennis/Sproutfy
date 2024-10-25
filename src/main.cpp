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

const int soilMoisturePins[] = {A2,A3,A4,A5};
const int pumpRelayPins[] = {1,2,3,4};

const int totalPages = 4;
unsigned long previousMillis = 0;
const long interval = 5000;
int currentPage = 0;

void setup() {
  Serial.begin(9600);

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha ao inicializar o display OLED!"));
    while (true);
  }
  display.clearDisplay();
  display.display();

  // Inicializa o sensor BME280
  if (!bme.begin(0x76)) {
    Serial.println(F("Falha ao inicializar sensor BME280!"));
    while (true);
  }

  // Inicializa o RTC
  if (!rtc.begin()) {
    Serial.println(F("RTC não encontrado!"));
    while (true);
  }
  if (rtc.lostPower()){
    Serial.println(F("RTC está sem energia, configurando para data e hora padrão!"));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  for (int i = 0; i < totalPages; i++) {
    pinMode(soilMoisturePins[i], INPUT);
    pinMode(pumpRelayPins[i], OUTPUT);
    digitalWrite(pumpRelayPins[i], HIGH);
  }

// Função para ler umidade do solo

}




