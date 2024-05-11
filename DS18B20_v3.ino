#include <microDS18B20.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//Настройка WiFi
const char* ssid = "Temperature";                 //Имя сети
const char* password = "tempnevga";               //Пароль

//Настройка IP адресса
IPAddress local_ip(192,168,43,186);               
IPAddress gateway(192,168,43,186);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

//Переменные для датчика
MicroDS18B20<14> sensor; 
int minValue;
int maxValue;
int flag = 0;                                    //Флаг для первого и второго определения мин и мах при включении 

//Таймер
unsigned long timer;                              //Для периодической записи температуры в массив

void setup() {
  //Запускается точка доступа
  WiFi.softAP(ssid, password);                    //Создается и настраивается сеть 
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  //Подключается обработчик, срабатывающий при открытии html страницы
  server.on("/", handle_OnConnect);
  server.begin();

  timer = millis();                  //Для периодической записи температуры в массив
}

void loop() {
  server.handleClient();                           //Ждет, когда к нему обратятся

  if (millis()-timer > 1000*60) {                  //Каждую минуту 
    SendHTML(); 
    timer = millis();                              //Вызывается фукнция отправки HTML страницы
  }                                                //в которой вызывается фукнция опроса датчика
}

int presentTemp() {
  //Функция записи актуального значения температуры
  int temp;
  sensor.requestTemp();                             //Отправляется запрос к датчику
  if (sensor.readTemp()) {                          //Если ответ датчика корректный
    delay(1000);                                    //(задержка в одну секунду для того, чтобы датчик произвел замер)
    temp = sensor.getTempInt();                     //То значение температуры сохраняется
  }
  if (flag < 2) {                                   //Если это первый/второй опрос датчика
    minValue = maxValue = temp;                     //То мин и макс будут равны первому/второму снятому значению
    flag += 1;
  }
  if (temp > maxValue) {                            //Если температура выше максимальной,
    maxValue = temp;                                //минимальная изменяется
  }
  if (temp < minValue) {                            //Аналогично с минимальной температурой
    minValue = temp;
  }
  return temp;
}

void handle_OnConnect() {
  //При переходе на корневую страницу, передается главная html страница
  server.send(200, "text/html", SendHTML());      //При переходе по IP адрессу датчика передается html страница
}

String SendHTML() {
  //html страница
  String page = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device\"><title>Nevga</title></head>";
  page += "<body style=\"margin: 0; background-image: url(https://encrypted-tbn0.gstatic.com/imagesq=tbn:ANd9GcS2jz9JWQQU8hQnh-MDT8u7Pwb4Ig-IW-B02N_itOyLAXR-CLvk1oqPbEP1uINqZV-_Vvk&usqp=CAU); background-size: 1000%;\">";
  page += "<div style=\"overflow: hidden; display: flex; flex-direction: column; min-height: 100vh; font-family: 'Times New Roman', Times, serif;\">";
  page += "<header style=\"margin: 100px;\"></header>";
  page += "<main style=\"text-align: center; margin: 0 auto;\"><table><tr><td colspan=\"2\" style=\"font-size: 400%;\">Temperature</td>";
  page += "</tr><tr><td colspan=\"2\" style=\"border: 6px solid black; border-radius: 40px 40px 0px 0px; font-size: 1400%;\">";
  page += presentTemp();                        //Вызывается фунция опроса датчика
  page += "°C</td></tr><tr><td style=\"border: 6px solid black; border-radius: 0px 0px 0px 40px; font-size: 400%;\">Min: ";
  page += minValue;
  page += "</td><td style=\"border: 6px solid black; border-radius: 0px 0px 40px 0px; font-size: 400%;\">Max: ";
  page += maxValue;
  page += "</td></tr></table></main><footer style=\"margin-top: auto; text-align: center; font-size: 300%;\">by Nevga</footer></div></body></html>";
  return page;
}
















