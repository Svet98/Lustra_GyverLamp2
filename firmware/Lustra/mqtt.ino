/*
void mqttSetupTopics () {  // добавляем к названию топика cfg.mqttID
DEBUGLN ("Start modifi topics");
char bufferTop[50];
  FOR_i(0, 5) {
    strcpy(bufferTop,cfg.mqttID);
    strcat(bufferTop,mqttTopic[i]);
    mqttTopic[i] = bufferTop;
    DEBUG ("Start topics ");
    DEBUG (i);
    DEBUG ("i = ");
    DEBUGLN (String(mqttTopic[i]));
  }  
     DEBUGLN ("Start topics i");
     FOR_i(0, 5) {
    DEBUG ("Debug topic ");
    DEBUG (i);
    DEBUG ("i = ");
       DEBUGLN (String(mqttTopic[i]));
     }
DEBUGLN (String(mqttTopic[7]));
}*/
//void mqtt_setup() {
//        //подписываемся на топик
//  DEBUGLN ("Start topics i");
//  FOR_i(0, 5) {
//    mqtt_client.subscribe(mqttTopic[i]);
//    DEBUGLN (String(mqttTopic[i]));
//  }  
//}

void reconnect() {
  char top[50];
  // Loop until we're reconnected
if (!mqtt_client.connected()) {
  Serial.println("Attempting MQTT REconnection...");
  String (topic) = String(cfg.mqttID) + "/status";
  topic.toCharArray(top, topic.length() + 1);

  if (mqtt_client.connect(cfg.mqttID, top, 1, true, "offline")) {
     mqtt_client.publish(top,"online", true);
// подписываемся на топики
    char bufferTop[50];
    FOR_i(0, 27) {    // получаем путь вида name/cmd/topic
      strcpy(bufferTop,cfg.mqttID);
      strcat(bufferTop,"/cmd");
      strcat(bufferTop,mqttTopic[i]);
      mqtt_client.subscribe(bufferTop);
//      DEBUG (String(mqttTopic[i]));
//      DEBUG (" = ");
//      DEBUGLN (String(bufferTop));
    }
     DEBUGLN ("MQTT is OK");  //CUR_PRES
    
  } else {
      Serial.print("failed, mqtt connect, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String strTopic = String(topic);             //конвертируем в стринг полученый топик
  String strPayload;   
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    strPayload += ((char)payload[i]);
  }
  Serial.println();

// скорость затухания/розжига белой LED
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[1])) {            //если получили с нужного топика
    cfg.led_speed = strPayload.toInt();
//    topicz = String(mqttTopic[0]);
    topicz = String(cfg.mqttID) + String(mqttTopic[0]);
//    DEBUG("topicz = ");
//    DEBUGLN(String(mqttTopic[1]));
//    DEBUGLN("#######");
    dataz = " Скорость w_led: " + String(cfg.led_speed);
    sendToMqtt();
  }
// яркость белой LED №1 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[2])) {     //если получили с нужного топика
    int (old_wBright) = cfg.led_w1Bright;      // сохраняем текущую яркость перед получением нового значения
    cfg.led_w1Bright = strPayload.toInt();
    cfg.led_w1Bright = cfg.led_w1Bright * 2.5;
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "Bright w1: " + String(old_wBright) + " -> " + String(cfg.led_w1Bright);
    led_white_fading(old_wBright, LED_PIN_W1, cfg.led_w1Bright);  // старая яркость, порт white ленты, полученная яркость
    sendToMqtt();
    EE_updCfg();
  }
// яркость белой LED №2
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[3])) {
    int (old_wBright) = cfg.led_w2Bright;
    cfg.led_w2Bright = strPayload.toInt();
    cfg.led_w2Bright = cfg.led_w2Bright * 2.5;
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "Bright w2: " + String(old_wBright) + " -> " + String(cfg.led_w2Bright);
    led_white_fading(old_wBright, LED_PIN_W2, cfg.led_w2Bright);
    sendToMqtt();
    EE_updCfg();
  }
// яркость белой LED №3
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[4])) {
    int (old_wBright) = cfg.led_w3Bright;
    cfg.led_w3Bright = strPayload.toInt();
    cfg.led_w3Bright = cfg.led_w3Bright * 2.5;
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "Bright w3: " + String(old_wBright) + " -> " + String(cfg.led_w3Bright);
    led_white_fading(old_wBright, LED_PIN_W3, cfg.led_w3Bright);  // старая яркость, порт white ленты, полученная яркость
    sendToMqtt();
    EE_updCfg();
  }
// RGB on/off
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[6])) {
    int (top_power) = strPayload.toInt();
        if (!cfg.state && top_power != 1) return;   // если лампа выключена и это не команда на включение - не обрабатываем
        switch (top_power) {
          case 0: controlHandler(0); 
            dataz = "Power RGB: off " ;
          break;               // выкл
          case 1: controlHandler(1); 
            dataz = "Power RGB: on " ;
          break;               // вкл
        }
    topicz = String(cfg.mqttID) + "/comand";
    sendToMqtt();
  } 
// Пресет
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[5])) {
  //  int (old_wBright) = strPayload.toInt();
    setPreset(strPayload.toInt());
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "Set  Preset: " + String(strPayload.toInt());
    sendToMqtt();
  }
// тип автосмены: 0 по порядку, 1 рандом
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[7])) {
  //  int (old_wBright) = strPayload.toInt();
    cfg.rotRnd = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "тип автосмены: 0 по порядку, 1 рандом: " + String(strPayload.toInt());
    sendToMqtt();
  }
// время смены пресета (1,5..)
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[8])) {
  //  int (old_wBright) = strPayload.toInt();
    cfg.rotPeriod = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "время смены пресета: " + String(strPayload.toInt());
    sendToMqtt();
  }
// смена пресетов: 0 ручная, 1 авто
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[9])) {
  //  int (old_wBright) = strPayload.toInt();
    cfg.rotation = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "смена пресетов: 0 ручная, 1 авто: " + String(strPayload.toInt());
    sendToMqtt();
  }
// режим ацп (1 выкл, 2 ярк, 3 муз)
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[10])) {
  //  int (old_wBright) = strPayload.toInt();
    cfg.adcMode = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "режим ацп (1 выкл, 2 ярк, 3 муз): " + String(strPayload.toInt());
    sendToMqtt();
  }
// глобальная яркость
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[11])) {
  //  int (old_wBright) = strPayload.toInt();
    cfg.bright = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "Set bright = " + String(strPayload.toInt());
    sendToMqtt();
  }
// cmd  -- запрос данных
// -- 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[26])) {
  //  cfg.led_wwBright = strPayload.toInt();
     int old_wBright = strPayload.toInt();
       if (old_wBright == 55) {
         mqttSendData();
       }
  }
// cmd  -- записать данные
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[26])) {
  //  cfg.led_wwBright = strPayload.toInt();
     int old_wBright = strPayload.toInt();
       if (old_wBright == 77) {
         EE_updCfg();
         topicz = String(cfg.mqttID) + "/comand";
         dataz = "Set Save cfg";
         sendToMqtt();
       }
  }
// ----- Работа с пресетом
// тип эффекта (1-3)
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[12])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.effect  = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "Set effect = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[13])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.fadeBright = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "флаг на свою яркость (0/1) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[14])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.bright = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "своя яркость (0.. 255) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[15])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.advMode = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "дополнительно (1,2...5) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[16])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.advMode = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "реакция на звук (1,2,3)  = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[17])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.min = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "мин сигнал светомузыки (0.. 255) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[18])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.max = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "макс сигнал светомузыки (0.. 255) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[19])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.speed = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "скорость (0.. 255) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[20])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.palette = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "палитра (1,2...) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[21])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.scale = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "масштаб (0.. 255) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[22])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.fromCenter = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "эффект из центра (0/1) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[22])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.color = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "эцвет (0.. 255) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }
// 
  if (strTopic == String(cfg.mqttID) + "/cmd" + String(mqttTopic[24])) {
  //  int (old_wBright) = strPayload.toInt();
    CUR_PRES.fromPal = strPayload.toInt();
    topicz = String(cfg.mqttID) + "/comand";
    dataz = "из палитры (0/1) = " + String(strPayload.toInt());
    sendToMqtt();
      EE_updatePreset();
      holdPresTmr.restart();
      loading = true;
  }


// Пресет полностью через cmd
/*    Serial.print(" Preset all: ");
    String temp = "GL,2,13,1,0,97,1,1,0,255,199,25,97,0,0,0,10,0,97,1,1,0,255,199,13,97,0,227,0,9,0,97,1,1,0,255,184,25,230,0,153,0,1,0,99,4,1,18,255,199,2,97,0,0,0,1,0,97,5,3,0,255,200,11,97,0,0,0,5,0,99,1,1,0,255,199,11,99,0,0,1,7,0,97,2,1,0,255,200,2,97,0,0,0,8,0,99,1,1,0,255,199,11,97,0,0,1,10,0,100,1,1,0,255,200,2,100,0,153,0,1,0,99,1,1,0,255,200,2,99,0,0,0,1,0,97,1,1,0,255,199,2,97,0,0,0,1,0,99,1,1,0,255,200,2,99,0,0,0,1,0,99,1,1,0,255,200,2,99,0,0,";
    temp += strPayload;
    strPayload = temp;
    Serial.println(String(strPayload));
  //  Udp.buf = temp;
  //  Udp.parsePacket(temp);
    mqtt_client.publish("lustra_zal/comand", " Preset all: ");
*/  

checkEEupdate();  //save EEPROM
}

// плавное гашение и розжиг white_led
void led_white_fading(int old_wBright, int LED_PIN, int new_wBright){  
  if (new_wBright > 255) {
    new_wBright == 255;
  }
  if (new_wBright < 0) {
    new_wBright == 0;
  }
  if (!new_wBright){
//    setPower(0);
  }
  if (new_wBright > old_wBright) {
    FOR_i(old_wBright, new_wBright + 1) {
        analogWrite(LED_PIN, i);
        delay(cfg.led_speed);
        DEBUG(i);
        DEBUG("   &  ");
        DEBUGLN(LED_PIN);
    }  
  } else {
    FOR_a(old_wBright, new_wBright - 1) {
        analogWrite(LED_PIN, a);
        delay(cfg.led_speed);
        DEBUG(a);
        DEBUG("   &  ");
        DEBUGLN(LED_PIN);
    }
  }
}
void viewPreset() {
//#include "data.h"         // данные

  DEBUGLN ("Состояние пресета:");
  DEBUGLN (CUR_PRES.effect);        // тип эффекта (1,2...) ВЫЧЕСТЬ 1
  DEBUGLN (CUR_PRES.fadeBright);    // флаг на свою яркость (0/1)
  DEBUGLN (CUR_PRES.bright);        // своя яркость (0.. 255)
  DEBUGLN (CUR_PRES.advMode);       // дополнительно (1,2...) ВЫЧЕСТЬ 1
  DEBUGLN (CUR_PRES.soundReact);    // реакция на звук (1,2...) ВЫЧЕСТЬ 1
  DEBUGLN (CUR_PRES.min);           // мин сигнал светомузыки (0.. 255)
  DEBUGLN (CUR_PRES.max);           // макс сигнал светомузыки (0.. 255)
  DEBUGLN (CUR_PRES.speed);       // скорость (0.. 255)
  DEBUGLN (CUR_PRES.palette);       // палитра (1,2...) ВЫЧЕСТЬ 1
  DEBUGLN (CUR_PRES.scale);       // масштаб (0.. 255)
  DEBUGLN (CUR_PRES.fromCenter);    // эффект из центра (0/1)
  DEBUGLN (CUR_PRES.color);         // цвет (0.. 255)
  DEBUGLN (CUR_PRES.fromPal);       // из палитры (0/1)
  DEBUGLN ("-------------");       // 
  DEBUGLN (cfg.presetAmount);       // кол-во режимов
  DEBUGLN (cfg.curPreset);       // текущий пресет
  DEBUGLN ("cocToRHNe preset end");       // 
//  DEBUGLN ();       // 
//  DEBUGLN ();       // 
//  DEBUGLN ();       // 
//  DEBUGLN ("Состояние пресета -end-");
  
}
void sendToMqtt(void) {
  char top[50];
  char dat[50];
  DEBUG ("Send mqqt: ");
  DEBUG (topicz);
//  String(tmp) = "["String(now.hour) + ":" + String(now.min) + ":" + String(now.sec) + "] " + dataz;
  topicz.toCharArray(top, topicz.length() + 1);
  dataz.toCharArray(dat, dataz.length() + 1);
  mqtt_client.publish(top, dat);
  DEBUG (" & ");
  DEBUGLN (dat);
  
}

void mqttSendData() {
// отправляем текущие данные
//      struct Preset
  dataz = String (CUR_PRES.effect);        // тип эффекта (1,2...) ВЫЧЕСТЬ 1
  topicz = String(cfg.mqttID) + String(mqttTopic[12]);
      sendToMqtt();
  dataz = String (CUR_PRES.fadeBright);    // флаг на свою яркость (0/1)
  topicz = String(cfg.mqttID) + String(mqttTopic[13]);
      sendToMqtt();
  dataz = String (CUR_PRES.bright);        // своя яркость (0.. 255)
  topicz = String(cfg.mqttID) + String(mqttTopic[14]);
      sendToMqtt();
  dataz = String (CUR_PRES.advMode);       // дополнительно (1,2...) ВЫЧЕСТЬ 1
  topicz = String(cfg.mqttID) + String(mqttTopic[15]);
      sendToMqtt();
  dataz = String (CUR_PRES.soundReact);    // реакция на звук (1,2...) ВЫЧЕСТЬ 1
  topicz = String(cfg.mqttID) + String(mqttTopic[16]);
      sendToMqtt();
  dataz = String (CUR_PRES.min);           // мин сигнал светомузыки (0.. 255)
  topicz = String(cfg.mqttID) + String(mqttTopic[17]);
      sendToMqtt();
  dataz = String (CUR_PRES.max);           // макс сигнал светомузыки (0.. 255)
  topicz = String(cfg.mqttID) + String(mqttTopic[18]);
      sendToMqtt();
  dataz = String (CUR_PRES.speed);       // скорость (0.. 255)
  topicz = String(cfg.mqttID) + String(mqttTopic[19]);
      sendToMqtt();
  dataz = String (CUR_PRES.palette);       // палитра (1,2...) ВЫЧЕСТЬ 1
  topicz = String(cfg.mqttID) + String(mqttTopic[20]);
      sendToMqtt();
  dataz = String (CUR_PRES.scale);       // масштаб (0.. 255)
  topicz = String(cfg.mqttID) + String(mqttTopic[21]);
      sendToMqtt();
  dataz = String (CUR_PRES.fromCenter);    // эффект из центра (0/1)
  topicz = String(cfg.mqttID) + String(mqttTopic[22]);
      sendToMqtt();
  dataz = String (CUR_PRES.color);         // цвет (0.. 255)
  topicz = String(cfg.mqttID) + String(mqttTopic[23]);
      sendToMqtt();
  dataz = String (CUR_PRES.fromPal);       // из палитры (0/1)
  topicz = String(cfg.mqttID) + String(mqttTopic[24]);
      sendToMqtt();
  DEBUGLN ("-------struct Preset end send------");       // 
//  dataz = String (cfg.presetAmount);       // кол-во режимов
//  topicz = String(cfg.mqttID) + String(mqttTopic[]);
//      sendToMqtt();
  dataz = String (cfg.curPreset);       // текущий пресет
  topicz = String(cfg.mqttID) + String(mqttTopic[5]);
      sendToMqtt();
  dataz = String (cfg.led_speed);
  topicz = String(cfg.mqttID) + String(mqttTopic[1]);
      sendToMqtt();
  dataz = String (cfg.led_w1Bright / 2.5);
  topicz = String(cfg.mqttID) + String(mqttTopic[2]);
      sendToMqtt();
  dataz = String (cfg.led_w2Bright / 2.5);
  topicz = String(cfg.mqttID) + String(mqttTopic[3]);
      sendToMqtt();
  dataz = String (cfg.led_w3Bright / 2.5);
  topicz = String(cfg.mqttID) + String(mqttTopic[4]);
      sendToMqtt();
  dataz = String (cfg.state);
  topicz = String(cfg.mqttID) + String(mqttTopic[6]);
      sendToMqtt();
  dataz = String (cfg.rotRnd);
  topicz = String(cfg.mqttID) + String(mqttTopic[7]);
      sendToMqtt();
  dataz = String (cfg.rotPeriod);
  topicz = String(cfg.mqttID) + String(mqttTopic[8]);
      sendToMqtt();
  dataz = String (cfg.rotation);
  topicz = String(cfg.mqttID) + String(mqttTopic[9]);
      sendToMqtt();
  dataz = String (cfg.adcMode);
  topicz = String(cfg.mqttID) + String(mqttTopic[10]);
      sendToMqtt();
  dataz = String (cfg.bright);
  topicz = String(cfg.mqttID) + String(mqttTopic[11]);
      sendToMqtt();
  dataz = String (cfg.workFrom);
  topicz = String(cfg.mqttID) + String(mqttTopic[29]);
      sendToMqtt();
  dataz = String (cfg.workTo);
  topicz = String(cfg.mqttID) + String(mqttTopic[30]);
      sendToMqtt();
  
}