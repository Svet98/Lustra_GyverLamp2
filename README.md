# Люстра GyverLamp2
Люстра на основе [проекта Алекса GyverLamp2.](https://github.com/AlexGyver/GyverLamp2)
<br>
![https://github.com/Svet98/Lustra_GyverLamp2](https://img.shields.io/badge/%D0%B2%20%D0%B2%D0%B5%D1%87%D0%BD%D0%BE%D0%B9-%D0%B4%D0%BE%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D0%BA%D0%B5-orange) ![https://github.com/Svet98/Lustra_GyverLamp2](https://img.shields.io/badge/Realese-No-yelow) ![https://github.com/Svet98/Lustra_GyverLamp2](https://img.shields.io/badge/ver-0.23b.2-blue)
<br>
:black_square_button: видео интеграции в МажорДоМо
[![ ](https://github.com/Svet98/MajorDoMo-R4S-teapot/blob/main/jpg/yt_logo_rgb_light.png?raw=true)](https://www.youtube.com/svet740)
<br>

- добавление трёх белых светодиодных 12v. лент
- mqtt
- ОТА через web (http://ip)
- интеграция в MajorDoMo
<br><br>
<b>...  изменения ...</b>
<details>
<summary>17.01.2023</summary>

- [x] добавлена отправка переменных по MQTT на сервер после включения питания
![image](https://user-images.githubusercontent.com/35732065/212833945-a87a3517-ce89-4862-9337-4ce8f2ff9682.png)

- [X] Добавлено управление временем работы RGB по MQTT
- (cfg.workFrom);           // часы работы (0,1.. 23)
- (cfg.workTo);             // часы работы (0,1.. 23)
  

</details>

<details>
<summary>17.11.2022</summary>
  - [x] добавлена сцена управления для МажорДоМо <br>

![Alt-текст](https://github.com/Svet98/Lustra_GyverLamp2/blob/main/docs/lustra.png?raw=true "сцена управления")
</details> 

<details>
<summary> 01.11.2022</summary><br>
  - MQTT</b>, добавлено (для отправки mqtt id/cmd/тема):

![image](https://user-images.githubusercontent.com/35732065/199292474-8662f5e3-4a14-426e-81f9-fad724c4b6d8.png)
<br>
  - <b>SSDP</b> добавлено отображение устройства в сети.<br>
![image](https://github.com/Svet98/Lustra_GyverLamp2/blob/main/docs/ssdp.png?raw=true)
</details>

<details>
<summary>30.10.2022...</summary>

  - [x] добавлен протокол mqtt для управления белыми лед лентами и основными функциями GyverLamp2 <br>
  - [x] mqtt сообщаяет состояние устройства в сети, LWT

</details> 
