#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#include "MQ135.h"
#include <LiquidCrystal.h> // LCD Kütüphanesi

LiquidCrystal lcd(D9, D8, D5, D4, D3, D2); // LCD  pinleri

MQ135 gasSensor = MQ135(A0);

#define DHT11PIN D6 // DHT11PIN olarak Dijital 2'yi belirliyoruz.

const char* ssid     = "paradox35";
const char* password = "SimonSchama35";

const char* host = "api.openweathermap.org";

float sicaklik, nem = 0;

float a0read = 0;
float havaResist = 0;
float havaPPM = 0;
float havaCPPM = 0;

String hava_durumu;
float hava_derecesi;

float UV_orani=0;

float hava_kirliligi;

int internet = 0;

double lastTime=0;


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
    lcdPrint("Internete ", "Bagli degil");
    delay(1500);
  }
  //lcdClear();

}

int value = 0;


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
        hava_derecesi=havaDerecesi;
        Serial.println(String(havaDerecesi) + " Derece ");
        
       
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
      {
        UV_orani = root1["value"];
       
        break;
      }
    }
  }


}

void havaKirliligi()
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
      {
        UV_orani = root1["value"];
        lcdPrint("UV Orani", "guncellendi");
       
        break;
      }
    }
  }


}
void loop() {

  if(internet)
  if (millis() - lastTime > 1000000)
  {
    havaDurumu();
    UVDurumu();
    lastTime = millis();    
  }

  ++value;
}
void tumVeriler()
{
  Serial.println("\n");
  // Sensörün okunup okunmadığını konrol ediyoruz.
  // chk 0 ise sorunsuz okunuyor demektir. Sorun yaşarsanız
  // chk değerini serial monitörde yazdırıp kontrol edebilirsiniz.
  int chk = DHT11.read(DHT11PIN);

  // Sensörden gelen verileri serial monitörde yazdırıyoruz.
  Serial.print("Nem (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("Sicaklik (Celcius): ");
  Serial.println((float)DHT11.temperature, 2);

  Serial.print("Sicaklik (Fahrenheit): ");
  Serial.println(DHT11.fahrenheit(), 2);

  Serial.print("Sicaklik (Kelvin): ");
  Serial.println(DHT11.kelvin(), 2);

  // Çiğ Oluşma Noktası, Dew Point
  Serial.print("Cig Olusma Noktasi: ");
  Serial.println(DHT11.dewPoint(), 2);

  sicaklik = DHT11.temperature;
  nem = DHT11.humidity;


  Serial.println(" ");
  int a0read = analogRead(A0);
  Serial.println(" A0 : " + String(a0read));
  Serial.println(" get resistance : " + String(gasSensor.getResistance()));
  a0read = analogRead(A0);
  havaResist = gasSensor.getResistance();
  havaPPM = gasSensor.getPPM();
  havaCPPM = gasSensor.getCorrectedPPM(sicaklik, nem);

  Serial.println(" get PPM : " + String(gasSensor.getPPM()));
  Serial.println(" get corrected PPM : " + String(gasSensor.getCorrectedPPM(sicaklik, nem)));
  Serial.println(" A0 : " + String(a0read));
  Serial.println(" get resistance : " + String(havaResist));


  delay(3000);

}

