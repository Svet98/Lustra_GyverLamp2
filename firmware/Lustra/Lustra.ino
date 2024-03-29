// ---------- Настройки -----------
#define GL_KEY "GL"         // ключ сети

// ------------ Кнопка -------------
#define BTN_PIN 2           // пин кнопки GPIO4 (D2 на wemos/node), 0 для схемы с ESP-01
#define USE_BTN 0           // 1 использовать кнопку, 0 нет

// ------------- АЦП --------------
#define USE_ADC 1           // можно выпилить АЦП
#define USE_CLAP 1          // два хлопка в ладоши вкл выкл лампу
#define MIC_VCC 15          // питание микрофона GPIO12 (D6 на wemos/node)
#define PHOT_VCC 16         // питание фоторезистора GPIO14 (D5 на wemos/node)

// ------------ Три белые ленты 12v -------------
//#define SKIP_white_LED 0    // включить 3 порта для белой ленты
#define LED_PIN_W2 13       // пин для канала white (боковая стенка)
#define LED_PIN_W3 14       // пин для канала worm white (боковая стенка)
#define LED_PIN_W1 12       // пин для канала white (центр)

// ------------ Лента -------------
#define STRIP_PIN 4         // пин ленты GPIO2 (D4 на wemos/node), GPIO5 (D1) для module
#define MAX_LEDS 300        // макс. светодиодов
#define STRIP_CHIP WS2812   // чип ленты
#define STRIP_COLOR GRB     // порядок цветов в ленте
#define STRIP_VOLT 5        // напряжение ленты, V
/*
  WS2811, GBR, 12V
  WS2812, GRB, 5V
  WS2813, GRB, 5V
  WS2815, GRB, 12V
  WS2818, RGB, 12V
*/

// ------------ WiFi AP ------------
const char AP_NameChar[] = "GyverLamp2";
const char WiFiPassword[] = "12345678";

// ------------ Прочее -------------
#define GL_VERSION 23       // код версии прошивки
#define EE_TOUT 30000       // таймаут сохранения епром после изменения, мс
#define DEBUG_SERIAL_LAMP   // закомментируй чтобы выключить отладку (скорость 115200)
#define EE_KEY 56           // ключ сброса eeprom
#define NTP_UPD_PRD 5       // период обновления времени с NTP сервера, минут
//#define SKIP_WIFI         // пропустить подключение к вафле (для отладки)

// ------------ MQTT ---------------
String(topicz);
String (dataz);

// ------------ БИЛДЕР -------------
#define GL_BUILD 0  // 0: com 300, 1: com 900, 2: esp1 300, 3: esp1 900, 4: module 300, 5: module 900

#if (GL_BUILD == 0)
#elif (GL_BUILD == 1)
#define MAX_LEDS 900
#elif (GL_BUILD == 2)
#define MAX_LEDS 300
#define BTN_PIN 0
#define STRIP_PIN 2
#define USE_ADC 0
#elif (GL_BUILD == 3)
#define MAX_LEDS 900
#define BTN_PIN 0
#define STRIP_PIN 2
#define USE_ADC 0
#elif (GL_BUILD == 4)
#define MAX_LEDS 300
#define STRIP_PIN 5
#elif (GL_BUILD == 5)
#define MAX_LEDS 900
#define STRIP_PIN 5
#endif

// ---------- БИБЛИОТЕКИ -----------
//#define FASTLED_ALLOW_INTERRUPTS 0
#include "data.h"         // данные
#include "Time.h"         // часы
#include "timeRandom.h"   // случайные числа по времени
//#include "fastRandom.h"   // быстрый рандом
#include "Button.h"       // библа кнопки
#include "palettes.h"     // палитры
#include "NTPClient-Gyver.h"  // сервер времени (модиф)
#include "timerMillis.h"  // таймер миллис
#include "VolAnalyzer.h"  // анализатор громкости
#include "FFT_C.h"        // фурье
#include <FastLED.h>      // лента
#include <ESP8266WiFi.h>  // базовая либа есп
#include <WiFiUdp.h>      // общение по UDP
#include <EEPROM.h>       // епром
#include "ESP8266httpUpdate.h"  // OTA
#include "mString.h"      // стринг билдер
#include "Clap.h"         // обработка хлопков
// add
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <PubSubClient.h>
// ------------------- ДАТА --------------------
//mqttConfTopic cfgTop;
Config cfg;
Preset preset[MAX_PRESETS];
Dawn dawn;
Palette pal;
//WiFiServer server(80);
ESP8266WebServer server(80);
WiFiUDP Udp;
WiFiUDP ntpUDP;
IPAddress broadIP;
NTPClient ntp(ntpUDP);
CRGB leds[MAX_LEDS];
Time now;
Button btn(BTN_PIN);
timerMillis EEtmr(EE_TOUT), turnoffTmr, connTmr(120000ul), dawnTmr, holdPresTmr(30000ul), blinkTmr(300);
timerMillis effTmr(30, true), onlineTmr(500, true), postDawn(10 * 60000ul);
TimeRandom trnd;
VolAnalyzer vol(A0), low, high;
FastFilter phot;
Clap clap;
// mqtt
WiFiClient ESPclient;
PubSubClient mqtt_client(ESPclient);
// add ota
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update1'><input type='submit' value='Update'></form>";
// SSDP
String SSDP_Name = cfg.mqttID; // Имя SSDP. Для обнаружения с компьютера через "сеть". Меняется в приложении, настройки MQTT, ID

uint16_t portNum;
uint32_t udpTmr = 0, gotADCtmr = 0;
byte btnClicks = 0, brTicks = 0;
unsigned char matrixValue[11][16];
bool gotNTP = false, gotTime = false;
bool loading = true;
int udpLength = 0;
byte udpScale = 0, udpBright = 0;

// ------------------- SETUP --------------------
void setup() {
  misc();
  delay(2000);          // ждём старта есп
#ifdef DEBUG_SERIAL_LAMP
  Serial.begin(115200);
  DEBUGLN();
#endif
  startStrip();         // старт ленты
  btn.setLevel(digitalRead(BTN_PIN));   // смотрим что за кнопка
  EE_startup();         // читаем епром
// white led
  pinMode(LED_PIN_W1, OUTPUT);
  pinMode(LED_PIN_W2, OUTPUT);
  pinMode(LED_PIN_W3, OUTPUT);
#ifndef SKIP_WIFI
  checkUpdate();        // индикация было ли обновление
  showRGB();            // показываем ргб
  checkGroup();         // показываем или меняем адрес
  checkButton();        // проверяем кнопку на удержание
  startWiFi();          // старт вайфай
  setupTime();          // выставляем время
if (cfg.WiFimode) {     // mqtt
//  #ifndef cfg.mqtt
//    mqttSetupTopics ();
    mqtt_client.setServer(cfg.mqttHost, cfg.mqttPort);
    mqtt_client.setCallback(callback);
//    mqtt_setup();    
//  #endif
}
  SSDP_init();          /// запускаем SSDP интерфейс
#endif
  setupOTA();           // OTA
  setupADC();           // настраиваем анализ
  presetRotation(true); // форсировать смену режима
}

void loop() {
  timeTicker();       // обновляем время
  yield();
#ifndef SKIP_WIFI
  tryReconnect();     // пробуем переподключиться если WiFi упал
  yield();
  parsing();          // ловим данные
  yield();
#endif
// mqtt
  if (cfg.mqtt) {
    if (!mqtt_client.connected()) {
      DEBUG("loop mqtt reconnect = ");
      DEBUGLN(!mqtt_client.connected());
      reconnect();
    // отправка значений в mqtt
    mqttSendData();
     
    }
  }
  if (mqtt_client.connected()) {
    mqtt_client.loop();
  }  
// end mqtt  

  checkEEupdate();    // сохраняем епром
  presetRotation(0);  // смена режимов по расписанию
  effectsRoutine();   // мигаем
  yield();
  button();           // проверяем кнопку
  checkAnalog();      // чтение звука и датчика
  yield();
  iAmOnline();
  server.handleClient();  // ota  
}
