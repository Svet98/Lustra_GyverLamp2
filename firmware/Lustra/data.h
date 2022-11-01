// -------------- ВНУТР. КОНСТАНТЫ ---------------
#define GL_ADC_NONE 1
#define GL_ADC_BRI 2
#define GL_ADC_MIC 3
#define GL_ADC_BOTH 4
#define GL_TYPE_STRIP 1
#define GL_TYPE_ZIG 2
#define GL_TYPE_PARAL 3
#define GL_ADV_NONE 1
#define GL_ADV_VOL 2
#define GL_ADV_LOW 3
#define GL_ADV_HIGH 4
#define GL_ADV_CLOCK 5
#define GL_REACT_BRI 1
#define GL_REACT_SCL 2
#define GL_REACT_LEN 3
#define GL_SLAVE 0
#define GL_MASTER 1
#define MAX_PRESETS 40      // макс количество режимов

// ------------------- МАКРО --------------------
#ifdef DEBUG_SERIAL_LAMP
#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x)
#else
#define DEBUGLN(x)
#define DEBUG(x)
#endif

#define FOR_a(x,y)  for (int a = (x); a > (y); a--)
#define FOR_i(x,y)  for (int i = (x); i < (y); i++)
#define FOR_j(x,y)  for (int j = (x); j < (y); j++)
#define FOR_k(x,y)  for (int k = (x); k < (y); k++)
#define CUR_PRES preset[cfg.curPreset]

byte scaleFF(byte x, byte b) {
  return ((uint16_t)x * (b + 1)) >> 8;
}
int mapFF(byte x, byte min, byte max) {
  return (((max - min) * x + (min << 8) + 1) >> 8);
}

const char OTAhost[] = "https://www.v-elite.ru/ota/";
const char *OTAfile[] = {
  "GL2_latest.bin",
  "com_300.bin",
  "com_900.bin",
  "esp1_300.bin",
  "esp1_900.bin",
  "module_300.bin",
  "module_900.bin",
};

const char NTPserver[] = "pool.ntp.org";
//"pool.ntp.org"
//"europe.pool.ntp.org"
//"ntp1.stratum2.ru"
//"ntp2.stratum2.ru"
//"ntp.msk-ix.ru"

#define PAL_SIZE 49
struct Palette {
  byte size = 1;
  byte strip[16 * 3];
};

#define CFG_SIZE 12
struct Config {
  byte bright = 100;      // яркость
  byte adcMode = 1;       // режим ацп (1 выкл, 2 ярк, 3 муз)
  byte minBright = 0;     // мин яркость
  byte maxBright = 255;   // макс яркость
  byte rotation = 0;      // смена режимов: 0 ручная, 1 авто
  byte rotRnd = 0;        // тип автосмены: 0 в порядке, 1 рандом
  byte rotPeriod = 1;     // период смены (1,5..)
  byte deviceType = 2;    // 1 лента, 2 зигзаг, 3 параллел
  byte maxCur = 5;        // макс ток (мА/100)
  byte workFrom = 0;      // часы работы (0,1.. 23)
  byte workTo = 0;        // часы работы (0,1.. 23)
  byte matrix = 1;        // тип матрицы 1.. 8

  int16_t length = 16;    // длина ленты
  int16_t width = 15;     // ширина матрицы

  byte GMT = 16;          // часовой пояс +13
  uint32_t cityID = 1;    // city ID
  bool mqtt = 1;          // mqtt
  char mqttID[32] = "lustra_zal";        //
  char mqttHost[32] = "192.168.1.11";      //
  int mqttPort = 1883;       //
  char mqttLogin[16];     //
  char mqttPass[16];      //

//  white led 3 strip 
  int SKIP_white_LED  = 1;
  int led_speed = 10;     //
  int led_w1Bright = 0;  //
  int led_w2Bright = 0; //  
  int led_w3Bright = 0; //

  byte state = 1;         // состояние 0 выкл, 1 вкл
  byte group = 1;         // группа девайса (1-10)
  byte role = 1;          // 0 slave, 1 master
  byte WiFimode = 1;      // 0 AP, 1 local
  byte presetAmount = 1;  // количество режимов
  byte manualOff = 0;     // выключали вручную?
  int8_t curPreset = 0;   // текущий режим
  int16_t minLight = 0;   // мин освещённость
  int16_t maxLight = 1023;// макс освещённость
  char ssid[32] = "v-elite";          // логин wifi
  char pass[32] = "ryzpm-98";          // пароль wifi
  byte version = GL_VERSION;
  byte update = 0;
};

#define PRES_SIZE 13
struct Preset {
  byte effect = 1;        // тип эффекта (1,2...) ВЫЧЕСТЬ 1
  byte fadeBright = 0;    // флаг на свою яркость (0/1)
  byte bright = 100;      // своя яркость (0.. 255)
  byte advMode = 1;       // дополнительно (1,2...) ВЫЧЕСТЬ 1
  byte soundReact = 1;    // реакция на звук (1,2...) ВЫЧЕСТЬ 1
  byte min = 0;           // мин сигнал светомузыки (0.. 255)
  byte max = 0;           // макс сигнал светомузыки (0.. 255)
  byte speed = 200;       // скорость (0.. 255)
  byte palette = 2;       // палитра (1,2...) ВЫЧЕСТЬ 1
  byte scale = 100;       // масштаб (0.. 255)
  byte fromCenter = 0;    // эффект из центра (0/1)
  byte color = 0;         // цвет (0.. 255)
  byte fromPal = 0;       // из палитры (0/1)
};

#define DAWN_SIZE 24
struct Dawn {
  byte state[7] = {0, 0, 0, 0, 0, 0, 0};  // (1/0)
  byte hour[7] = {0, 0, 0, 0, 0, 0, 0};   // (0.. 59)
  byte minute[7] = {0, 0, 0, 0, 0, 0, 0}; // (0.. 59)
  byte bright = 100;      // (0.. 255)
  byte time = 1;          // (5,10,15,20..)
  byte post = 1;          // (5,10,15,20..)
};

/*
  - Каждые 5 минут лампа AP отправляет время (день час минута) на Local лампы всех ролей в сети с ней (GL,6,день,час,мин)
  - Если включен АЦП, Мастер отправляет своей группе данные с него на каждой итерации отрисовки эффектов (GL,1,длина,масштаб,яркость)
  - Установка времени с мобилы - получают все роли АР и Local (не получившие ntp)
  - Каждую секунду устройства шлют посылку GL_ONL
*/
// mqtt topics
const char *mqttTopic[] = {
  "/comand",
  "/wledSpeed",
  "/w1Bright",
  "/w2Bright",
  "/w3Bright",
  "/Preset",  //5
  "/power",
  "/rotRnd",
  "/rotPeriod",
  "/rotation",
  "/adcMode",     // 10
  "/bright",
  "/PRESeffect",  //12
  "/PRESfadeBright",
  "/PRESbright",
  "/PRESadvMode",
  "/PRESsoundReact",
  "/PRESmin",
  "/PRESmax",
  "/PRESspeed",
  "/PRESpalette",   // 20
  "/PRESscale",
  "/PRESfromCenter",
  "/PREScolor",
  "/PRESfromPal",  //24
  "/curPreset",
  "/cmd", //26
  "/deviceType", //27
  "/maxCur", //28
  "/workFrom", //29
  "/workTo", //30
  "/matrix", //31
  "/width", //32
  "/GMT", //33
  "/cityID", //34
  "/mqttID", //35
  "/cmd", //36
  
};

/*
  #define topic_rotRnd ("lustra_zal/rotRnd")        // тип автосмены: 0 в порядке, 1 рандом
  #define topic_rotPeriod ("lustra_zal/rotPeriod")  // период смены (1,5..)
  #define topic_rotation ("lustra_zal/rotation")    // смена режимов: 0 ручная, 1 авто
  #define topic_adcMode ("lustra_zal/adcMode")      // режим ацп (1 выкл, 2 ярк, 3 муз)
  #define topic_bright ("lustra_zal/bright")        // яркость
  #define topic_PRESeffect ("lustra_zal/PRESeffect")
// пресет
  #define topic_PRESfadeBright ("lustra_zal/PRESfadeBright")    // флаг на свою яркость (0/1)
  #define topic_PRESbright ("lustra_zal/PRESbright")        // своя яркость (0.. 255)
  #define topic_PRESadvMode ("lustra_zal/PRESadvMode")       // дополнительно (1,2...) ВЫЧЕСТЬ 1
  #define topic_PRESsoundReact ("lustra_zal/PRESsoundReact")    // реакция на звук (1,2...) ВЫЧЕСТЬ 1
  #define topic_PRESmin ("lustra_zal/PRESmin")           // мин сигнал светомузыки (0.. 255)
  #define topic_PRESmax ("lustra_zal/PRESmax")           // макс сигнал светомузыки (0.. 255)
  #define topic_PRESspeed ("lustra_zal/PRESspeed")       // скорость (0.. 255)
  #define topic_PRESpalette ("lustra_zal/PRESpalette")       // палитра (1,2...) ВЫЧЕСТЬ 1
  #define topic_PRESscale ("lustra_zal/PRESscale")       // масштаб (0.. 255)
  #define topic_PRESfromCenter ("lustra_zal/PRESfromCenter")    // эффект из центра (0/1)
  #define topic_PREScolor ("lustra_zal/PREScolor")         // цвет (0.. 255)
  #define topic_PRESfromPal ("lustra_zal/PRESfromPal")       // из палитры (0/1)
  #define topic_curPreset ("lustra_zal/curPreset")       // текущий пресет
 */
