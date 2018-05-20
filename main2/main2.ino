#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#include "MQ135.h"
#include <LiquidCrystal.h> // LCD Kütüphanesi

LiquidCrystal lcd(D9, D8, D5, D4, D3, D2); // LCD  pinleri

MQ135 gasSensor = MQ135(A0);

#define DHT11PIN D6 // DHT11PIN olarak Dijital 2'yi belirliyoruz.

const int button1Pin = D10;
const int button2Pin = D11;

//const char* ssid     = "Alihan";
//const char* password = "12345678";

const char* ssid     = "paradox35";
const char* password = "SimonSchama35";

//const char* ssid     = "MAVISEHIR_BILIM_LOBI_2";
//const char* password = "Doga204060!";

//const char* ssid     = "ilkeriphone";
//const char* password = "12345679";

const char* host = "api.openweathermap.org";

float sicaklik, nem = 0;

float a0read = 0;
float havaResist = 0;
float havaPPM = 0;
float havaCPPM = 0;

String hava_durumu = "";
float hava_derecesi = 0;

float UV_orani = 0;

float hava_kirliligi;

int internet = 0;

double lastTime = 0;
double ekranTimer = 0;
double ortamTimer = 0;

int mod = 0;
int pressed = 0;

String sicaklikCumle[20];
int sicaklikAdet = 0;

String nemCumle[20];
int nemAdet = 0;

String havaKaliteCumle[20];
int havaKaliteAdet = 0;

String havaCumle[20];
int havaAdet = 0;

String UVCumle[20];
int UVAdet = 0;

int yazi_durum = 0;

StaticJsonBuffer<200> jsonBuffer;
dht11 DHT11;


void lcdClear()
{
  lcd.setCursor(0, 0);
  lcd.print("                 ");
  lcd.setCursor(0, 1);
  lcd.print("                 ");
}
void lcdPrint(String text1, String text2 = "                 ")
{
  lcdClear();
  lcd.setCursor(0, 0);
  lcd.print(String(text1));
  lcd.setCursor(0, 1);
  lcd.print(String(text2));
}

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  delay(10);

  pinMode(button1Pin, OUTPUT);
  pinMode(button2Pin, OUTPUT);


  lcd.print("Loading");

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  String mt = ".";
  int tryadd = 0;
  while (WiFi.status() != WL_CONNECTED) {

    lcdPrint("Connecting to ", String(mt));
    delay(500);
    tryadd++;
    if (tryadd > 8)
      break;
    mt = (String)mt + String(".");

  }
  if (tryadd < 9)
  {
    internet = 1;
    lcdPrint("Connected ", String(ssid));
    delay(1500);

    lcdPrint("Web server'a ", "baglaniliyor");
    havaDurumu();
    lcdPrint("Hava Durumu ", "guncellendi");
    delay(1500);
    UVDurumu();
    lcdPrint("UV Orani ", "guncellendi");
  } else
  {
    UVCumle[0] = "UV Oranina|Ulasilamiyor";
    lcdPrint("Internete ", "Bagli degil");
    delay(1500);
  }

  ortamVeriGuncelle();
  //lcdClear();

}

void havaDurumu()
{

  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {

    lcdPrint("Baglanti", "basarisiz");
    Serial.println("connection failed");
    return;
  }

  String url = "/data/2.5/weather?q=Izmir,TR&appid=5485a5c443e83636374329f55e49120d";


  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      //Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line.indexOf("coord") > 0)
    {


      String satir = line;
      //Serial.println(satir);

      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& root1 = jsonBuffer.parseObject(satir);
      if (root1.success())
      {
        String havaDurumuEn = root1["weather"][0]["main"];
        float havaDerecesi = root1["main"]["temp"];


        havaDerecesi -= 273, 15;
        hava_derecesi = havaDerecesi;
        if (havaDurumuEn == "Clouds")
        {
          hava_durumu = "Bulutlu";
        } else if (havaDurumuEn == "Clear")
        {
          hava_durumu = "Hava Acik";
        } else hava_durumu = havaDurumuEn;
        break;
      }
    }
  }


}

void UVDurumu()
{

  //lcdPrint("Web server'a ", "baglaniliyor");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {

    lcdPrint("Baglanti", "basarisiz");
    Serial.println("connection failed");
    return;
  }

  String url = "/data/2.5/uvi?lat=38.423&lon=27.142&appid=5485a5c443e83636374329f55e49120d";


  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      //Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if (line.indexOf("date_iso") > 0)
    {
      String satir = line;

      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& root1 = jsonBuffer.parseObject(satir);
      if (root1.success())
        UVAdet = 1;
      {
        UV_orani = root1["value"];
        if (UV_orani < 3)
        {
          UVAdet++;
          UVCumle[1] = "Dusuk UV orani|zararsiz";
        } else if (UV_orani < 6) {
          UVAdet++;
          UVCumle[1] = "Orta UV orani|zararsiz";

        } else if (UV_orani < 8) {
          UVAdet++;
          UVCumle[1] = "Yuksek UV orani|zararsiz";

        } else if (UV_orani < 11) {
          UVAdet += 4;
          UVCumle[1] = "Cok Riskli|Yuksek UV orani";
          UVCumle[2] = "Gunes ile kisa |sureli dahi";
          UVCumle[3] = "temas edilirse|gozde ve deride";
          UVCumle[4] = "ciddi hasarlar|olusabilir";
        } else {

        }

        break;
      }
    }
  }
}

void aciklama(String cumleler[20], int adet)
{
  yazi_durum = 1;
  delay(100);
  for (int i = 0; i < adet; i++)
  {
    String cumle = cumleler[i];
    int cumle_adet = cumle.length();
    String firstVal, secondVal;

    for (int j = 0; j < cumle.length(); j++)
    {
      if (cumle.substring(j, j + 1) == "|") {
        firstVal = cumle.substring(0, j);
        secondVal = cumle.substring(j + 1, cumle_adet);
        break;
      }
    }

    lcdPrint(firstVal, (secondVal + "."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);lcdPrint(firstVal, (secondVal + "."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);lcdPrint(firstVal, (secondVal + "."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);lcdPrint(firstVal, (secondVal + "."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);
    lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);
    lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100);lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100);lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100);lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100);
  }


}
void ortamVeriGuncelle()
{
  int chk = DHT11.read(DHT11PIN);

  //  Serial.print("Nem (%): ");
  //  Serial.println((float)DHT11.humidity, 2);
  //
  //  Serial.print("Sicaklik (Celcius): ");
  //  Serial.println((float)DHT11.temperature, 2);

  sicaklik = DHT11.temperature;
  nem = DHT11.humidity;

  int a0read = analogRead(A0);
  havaResist = gasSensor.getResistance();
  havaPPM = gasSensor.getPPM();
  havaCPPM = gasSensor.getCorrectedPPM(sicaklik, nem);
  if (a0read > 400) {
    mod = 99;
  } else if (mod == 99)
  {
    mod = 0;

  }
  //  Serial.println(" get PPM : " + String(gasSensor.getPPM()));
  //  Serial.println(" get corrected PPM : " + String(gasSensor.getCorrectedPPM(sicaklik, nem)));
  //  Serial.println(" A0 : " + String(a0read));
  //  Serial.println(" get resistance : " + String(havaResist));

}

void ekran_tazele()
{
  if (mod == 0)
  {
    lcdPrint(String(int(sicaklik)) + "C | Nem %" + String(int(nem)), hava_durumu + " " + String(hava_derecesi) + "C");
  } else if (mod == 1)
  {
    lcdPrint("Ortam Sicaklik " , String(sicaklik) + "C");
  } else if (mod == 2)
  {
    lcdPrint("Nem %" + String(int(nem)));
  } else if (mod == 3)
  {
    lcdPrint("Ortam Hava", "Kalitesi " + String(havaCPPM));
  } else if (mod == 4)
  {
    lcdPrint("Hava Durumu ", hava_durumu + " " + String(hava_derecesi) + "C");
  } else if (mod == 5)
  {
    UVCumle[0] = "izmir gunes|UV Orani " + String(UV_orani);

    lcdPrint("izmir gunes", "UV Orani " + String(UV_orani));
    if (yazi_durum == 0 && pressed==0)
    {
      aciklama(UVCumle, UVAdet);
    }
  }


  if (mod == 99)
  {
    lcdPrint("Gaz Uyarisi", "Ortamdan Uzaklas!");

  }


}
void loop()
{
  int buttonState1 = digitalRead(button1Pin);
  int buttonState2 = digitalRead(button2Pin);
  //Serial.println(" buton 1 " + String(buttonState1) + " | buton 2 " + String(buttonState2));
  if (buttonState1 == 1)
  {
    if (pressed == 0)
    {
      pressed = 1;
      if (mod != 0)
      {
        mod = 0;
        ekran_tazele();
      }
    }
  } else if (buttonState2 == 1)
  {
    if (pressed == 0)
    {
      yazi_durum = 0;
      pressed = 1;
      mod++;
      if (mod > 5 && !(mod > 90 && mod < 100)) mod = 0;
      ekran_tazele();
    }
  } else pressed = 0;
  delay(100);

  if (internet)
    if (millis() - lastTime > 1000000)
    {
      havaDurumu();
      UVDurumu();
      lastTime = millis();
    };
  if (millis() - ortamTimer > 10000)
  {
    ortamTimer = millis();
    ortamVeriGuncelle();
  }
  if (millis() - ekranTimer > 1000)
  {
    ekranTimer = millis();
    ekran_tazele();
  }
}

