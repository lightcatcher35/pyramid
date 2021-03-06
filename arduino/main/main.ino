#include <time.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#include "MQ135.h"
#include <LiquidCrystal.h> // LCD Kütüphanesi

LiquidCrystal lcd(D9, D8, D5, D4, D3, D2); // LCD  pinleri

MQ135 gasSensor = MQ135(A0);

#define DHT11PIN D6 // DHT11PIN olarak Dijital 2'yi belirliyoruz.

const int button1Pin = D10;
const int button2Pin = D11;

const char* ssid     = "paradox35";
const char* password = "SimonSchama35";

//const char* ssid     = "ilkeriphone";
//const char* password = "12345679";

//const char* ssid     = "BilisimLab";
//const char* password = "bilisim@34!";

const char* host = "api.openweathermap.org";

#define FIREBASE_HOST "pyramid-19329.firebaseio.com"
#define FIREBASE_AUTH "j1yT07XvjNGpTOll8eAGcOoWWJV963myKXhn1xHy"


StaticJsonBuffer<250> jsonBuffer;
dht11 DHT11;

float sicaklik, nem = 0;

float a0read = 0;
float havaResist = 0;
float havaPPM = 0;
float havaCPPM = 0;

String hava_durumu = "";
float hava_derecesi = 0;

float UV_orani = 0;

float hava_kirliligi;

int internet,n = 0;

double lastTime = 0;
double ekranTimer = 0;
double ortamTimer = 0;

double updateTimer = 0;
double updateHalfTimer = 0;

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

String saat="";
String tarih="";
String gun="";
String ay="";
String yil="";

JsonObject& root = jsonBuffer.createObject();

int yazi_durum = 0;

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
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
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
    Serial.print(".");
    lcdPrint("Connecting to ", String(mt));
    delay(500);
    tryadd++;
    if (tryadd > 12)
      break;
    mt = (String)mt + String(".");
  }
  if (tryadd < 13)
  {
    internet = 1;
    Serial.println("İnternete bağlandı.");
    
    Serial.println("Web server'a  baglaniliyor.");
    Serial.println("Hava durumu alınıyor");
    havaDurumu();
    lcdPrint("Hava Durumu ", "guncellendi");
    delay(1500);
    
    Serial.println("UV alınıyor");
    UVDurumu();
    lcdPrint("UV Orani ", "guncellendi");    
    delay(1500);
    
    Serial.println("Firebase'e bağlanılıyor");
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    lcdPrint("Bulut Server'a", "Baglaniliyor");
    delay(1500);
    
  } else
  {
    UVCumle[0] = "UV Oranina|Ulasilamiyor";
    lcdPrint("Internete ", "Bagli degil");
    delay(1500);
  }

  ortamVeriGuncelle();

  
  configTime(3 * 3600, 0, "tr.pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("ok");
  time_t now = time(nullptr);
}
String ayVeri(String ay)
{
  if(ay=="Jan")
  {
    return "Ock";
  }else if(ay=="Feb")
  {
    return "Şub";
  }else if(ay=="Mar")
  {
    return "Mrt";
  }else if(ay=="Apr")
  {
    return "Nis";
  }else if(ay=="May")
  {
    return "May";
  }else if(ay=="Jun")
  {
    return "Haz";
  }else if(ay=="Jul")
  {
    return "Tem";
  }else if(ay=="Aug")
  {
    return "Ağs";
  }else if(ay=="Sep")
  {
    return "Eyl";
  }else if(ay=="Oct")
  {
    return "Ekm";
  }else if(ay=="Nov")
  {
    return "Kas";
  }else if(ay=="Dec")
  {
    return "Ara";
  }else 
  {
    return ay;
  }
}
void zamaniGuncelle()
{
  time_t now = time(nullptr);

  //Serial.println(ctime(&now));
  String part01 = getValue(String(ctime(&now)),' ',0);
  String part02 = getValue(String(ctime(&now)),' ',1);
  String part03 = getValue(String(ctime(&now)),' ',2);
  String part04 = getValue(String(ctime(&now)),' ',3);
  String part05 = getValue(String(ctime(&now)),' ',4);
  String part06 = getValue(String(ctime(&now)),' ',5);
  
  yil = getValue(String(ctime(&now)),' ',5);
  ay=ayVeri(part02);
  if(part03.length())
  {
    tarih =String(part03+" "+ay);
    saat =part04;
    
  }else
  {
    tarih =String(part04+" "+ay);
   saat =part05;
  }


  //Serial.println(tarih);
  //Serial.println(saat);
  
}

void havaDurumu()
{

  if (internet == 0)
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    String mt = ".";
    int tryadd = 0;
    while (WiFi.status() != WL_CONNECTED) {

      delay(500);
      tryadd++;
      if (tryadd > 12)
        break;
      mt = (String)mt + String(".");
    }
    if (tryadd < 13)
    {
      internet = 1;
    }
  }
  if (internet == 1)
  {

    //Serial.println(host);
    
    Serial.println("hava durumu için servise bağlanılıyor");

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
      if (millis() - timeout > 7000) {
        Serial.println(">>> Client Timeout !");
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
          
          Serial.println("hava durumu alındı.");
          
          String havaDurumuEn = root1["weather"][0]["main"];
          
          //Serial.println(havaDurumuEn);
          float havaDerecesi = root1["main"]["temp"];


          havaDerecesi -= 273, 15;
          hava_derecesi = havaDerecesi;
          if (havaDurumuEn == "Clouds")
          {
            hava_durumu = "Bulutlu";
          } else if (havaDurumuEn == "Rain")
          {
            hava_durumu = "Yagmurlu";
          } else if (havaDurumuEn == "Mist")
          {
            hava_durumu = "Sisli";
          }  else if (havaDurumuEn == "Haze")
          {
            hava_durumu = "Puslu";
          }  else if (havaDurumuEn == "Fog")
          {
            hava_durumu = "Sisli";
          } else if (havaDurumuEn == "Clear")
          {
            hava_durumu = "Hava Acik";
          } else hava_durumu = havaDurumuEn;
          break;
        }else
        {
          
          Serial.println(">>> Client Timeout !");
        }
      }
    }
  }


}

void UVDurumu()
{
  if (internet == 0)
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    String mt = ".";
    int tryadd = 0;
    while (WiFi.status() != WL_CONNECTED) {

      delay(500);
      tryadd++;
      if (tryadd > 12)
        break;
      mt = (String)mt + String(".");

    }
    if (tryadd < 13)
    {
      internet = 1;
    }
  }

  if (internet == 1)
  {
    //lcdPrint("Web server'a ", "baglaniliyor");
    //Serial.println(host);

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
          Serial.println("UV Parse Edildi");
          UVAdet = 1;
          UV_orani = root1["value"];
          if (UV_orani < 4)
          {
            UVAdet += 2;
            UVCumle[1] = "Zararsiz UV|Orani";
            UVCumle[2] = "Zarar gorme riski|cok az";

          } else if (UV_orani < 6) {
            UVAdet += 4;
            UVCumle[1] = "Ortalama UV orani|Az riskli";
            UVCumle[2] = "Gunese saatlerce|maruz kalmamasi";
            UVCumle[3] = "Disinda|zararsiz";
            UVCumle[4] = "Onerilenler;|Gunes Gozlugu";
          } else if (UV_orani < 8) {
            UVAdet += 5;
            UVCumle[1] = "Yuksek UV orani| ";
            UVCumle[2] = "Gunes ile uzun|sureli temas";
            UVCumle[3] = "halinde goz|ve cilt zarar";
            UVCumle[4] = "Gorebilir|Onerilenler;";
            UVCumle[5] = "Gunes Korumali|Gozluk, Kiyafet";

          } else if (UV_orani < 11) {
            UVAdet += 4;
            UVCumle[1] = "Cok Riskli|Yuksek UV orani";
            UVCumle[2] = "Gunes ile kisa |sureli dahi";
            UVCumle[3] = "temas edilirse|gozde ve deride";
            UVCumle[4] = "ciddi hasarlar|olusabilir";
          } else if (UV_orani >= 11) {
            UVAdet += 5;
            UVCumle[1] = "Dikkat!|Asiri riskli UV";
            UVCumle[2] = "Gunes ile temas |dakikalar icinde";
            UVCumle[3] = "gozu ve deriyi|yakar!";
            UVCumle[4] = "Onerilenler|Evden cikmayin";
            UVCumle[5] = "Kapali alanda|kalin";

          } else

            break;
        }
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
    delay(200); lcdPrint(firstVal, (secondVal + "."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200); lcdPrint(firstVal, (secondVal + "."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200); lcdPrint(firstVal, (secondVal + "."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);
    lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200); lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200); lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200); lcdPrint(firstVal, (secondVal + ".."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(200);
    lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100); lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100); lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100); lcdPrint(firstVal, (secondVal + "..."));
    if (digitalRead(button1Pin) == 1 || digitalRead(button2Pin) == 1) break;
    delay(100);
  }


}
void ortamVeriGuncelle()
{
  int chk = DHT11.read(DHT11PIN);

  sicaklik = DHT11.temperature;
  nem = DHT11.humidity;

  a0read = analogRead(A0);
  havaResist = gasSensor.getResistance();
  havaPPM = gasSensor.getPPM();
  havaCPPM = gasSensor.getCorrectedPPM(sicaklik, nem);
  if (a0read > 450) {
    mod = 99;
  } else if (mod == 99)
  {
    mod = 0;

  }

  if (internet && sicaklik < (hava_derecesi - 9))
  {
    sicaklikAdet = 9;
    sicaklikCumle[1] = "ic dis sicaklik|farki yuksek";
    sicaklikCumle[2] = "Düşük derece klima|kullanımından";
    sicaklikCumle[3] = "gelebilecek zarar|listesi;";
    sicaklikCumle[4] = "Baş ağrısı|Halsizlik";
    sicaklikCumle[5] = "Baş dönmesi|denge kaybı";
    sicaklikCumle[6] = "Klimalarda her|derece farki";
    sicaklikCumle[7] = "artisi, %10 |fazla enerji";
    sicaklikCumle[8] = "gideri olarak |yansir";
  } else if (internet && hava_derecesi > 21 && sicaklik < 21)
  {
    sicaklikAdet = 10;
    sicaklikCumle[1] = "klima cok dusuk |derecelerde";
    sicaklikCumle[2] = "Bu durum kas|tutulmasi ve";
    sicaklikCumle[3] = "soguk alginligi|gibi sorunlara";
    sicaklikCumle[4] = "sebep olabilir|Ayrica;";
    sicaklikCumle[5] = "klimanin surekli|calismasi";
    sicaklikCumle[6] = "arizalara|sebep olabilir";
    sicaklikCumle[7] = "Klimalarda her|derece farki";
    sicaklikCumle[8] = "artisi, %10 |fazla enerji";
    sicaklikCumle[9] = "gideri olarak |yansir";

  } else if (internet && hava_derecesi < 18 && sicaklik > 24)
  {
    sicaklikAdet = 4;
    sicaklikCumle[1] = "isitici yuksek |derecelerde";
    sicaklikCumle[2] = "bu durum yuksek|enerji giderleri";
    sicaklikCumle[3] = "olusturabilir|";

  }
  if (nem<30) {
    nemAdet = 9;
    nemCumle[1] = "Hava fazla kuru|dusuk nemin";
    nemCumle[2] = "neden oldugu|zararlar";
    nemCumle[3] = "Bogaz kurulugu,|Gozlerde yanma";
    nemCumle[4] = "Havadaki nem|oranini";
    nemCumle[5] = "arttirmak icin|yapilabilecekler:";
    nemCumle[6] = "Ortami|havalandirmak";
    nemCumle[7] = "Nem arttirici|cihazlar";
    nemCumle[8] = "kullanmak ise|yarayacaktir";
    
  }
  if (nem>60) {
    nemAdet = 9;
    nemCumle[1] = "Hava fazla nemli|nem oraninin";
    nemCumle[2] = "verebilecegi|zararlar";
    nemCumle[3] = "Bunalticilik|hissi";
    nemCumle[4] = "Uyku hali|yaratabilir";
    nemCumle[5] = "azaltmak icin|yapilabilecekler:";       
    nemCumle[6] = "Ortami|havalandirmak";
    nemCumle[7] = "Klimalarin|nem alma modunu";
    nemCumle[8] = "calistirmak ise|yarayacaktir";
  }

  if(a0read<200)
  {
    havaKaliteAdet = 2;
    havaKaliteCumle[1] = "Ortam havasi|Uygun degerde";
  }else if(a0read<301)
  {
    havaKaliteAdet = 5;
    havaKaliteCumle[1] = "Hava kalitesi|istenilen aralikta";
    havaKaliteCumle[2] = "ama daha cok|havalandirma";
    havaKaliteCumle[3] = "yaparak|saglikli bir";
    havaKaliteCumle[4] = "hava degeri elde|edebilirsiniz";
  }else if(a0read<700)
  {
    havaKaliteAdet = 6;
    havaKaliteCumle[1] = "Ortam havasiz.|Sigara, ";
    havaKaliteCumle[2] = "duman yada|acik kalan ";
    havaKaliteCumle[3] = "bir gaz kaynagi|olabilir. ";
    havaKaliteCumle[4] = "Daha rahat bir|hava icin ";
    havaKaliteCumle[5] = "ortami|havalandiriniz ";
  }else 
  {
    havaKaliteAdet = 6;
    havaKaliteCumle[1] = "Hava kalitesi|tehlikeli ";
    havaKaliteCumle[2] = "degerlerde.| Odayi ";
    havaKaliteCumle[3] = "havalandirmali|yada ayni ortamda ";
    havaKaliteCumle[4] = "uzun sure|kalmamalisiniz";
  }
  
  // && sicaklik<21

  //Serial.println(" get PPM : " + String(gasSensor.getPPM()));
  //Serial.println(" get corrected PPM : " + String(gasSensor.getCorrectedPPM(sicaklik, nem)));
  //Serial.println(" A0 : " + String(a0read));
  //Serial.println(" get resistance : " + String(havaResist));

}

void ekran_tazele()
{
  if (mod == 0)
  {
    lcdPrint("Oda " + String(int(sicaklik)) + "C Nem %" + String(int(nem)), hava_durumu + " " + String(int(hava_derecesi)) + "C");
  } else if (mod == 1)
  {
    sicaklikCumle[0] = "Ortam Sicaklik|" + String(int(sicaklik)) + "C";

    lcdPrint("Ortam Sicaklik" , String(int(sicaklik)) + "C");
    if (yazi_durum == 0 && pressed == 0) aciklama(sicaklikCumle, sicaklikAdet);

  } else if (mod == 2)
  {
    lcdPrint("Nem %" + String(int(nem)));
    nemCumle[0] = "Nem %" + String(int(nem)) + "|";
    if (yazi_durum == 0 && pressed == 0) aciklama(nemCumle, nemAdet);
  } else if (mod == 3)
  {
    lcdPrint("Ortam Hava", "Kalitesi " + String(havaCPPM));
    nemCumle[0] = "Ortam Hava|Kalitesi " + String(havaCPPM)+"|";
    if (yazi_durum == 0 && pressed == 0) aciklama(havaKaliteCumle, havaKaliteAdet);
  } else if (mod == 4)
  {
    lcdPrint("Hava Durumu ", hava_durumu + " " + String(int(hava_derecesi)) + "C");
  } else if (mod == 5)
  {
    UVCumle[0] = "Gunes isini|UV Orani " + String(UV_orani);

    lcdPrint("Gunes isini", "UV Orani " + String(UV_orani));
    if (yazi_durum == 0 && pressed == 0)
    {
      aciklama(UVCumle, UVAdet);
    }
  }


  if (mod == 99)
  {
    lcdPrint("Gaz Uyarisi", "Ortamdan Uzaklas!");

  }


}
void firebaseHalfUpdate()
{

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root4 = jsonBuffer.createObject();
  
  root4["nem"] = nem;
  root4["sicaklik"] = sicaklik;
  root4["hava_kalitesi"] = a0read;
  root4["hava_derecesi"] = int(hava_derecesi);
  root4["uv_isini"] = UV_orani;
  root4["tarih"] = tarih;
  root4["saat"] = saat;
  
  Serial.println("");
  Serial.println("");
  Serial.println("Firebase'e yarım saatlik güncelleme yapılıyor");
  Serial.println(saat);
  Serial.println(tarih);
  root4.printTo(Serial);
  
  String name = Firebase.push("halfHourLogs", root4);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /Hourlogs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  //Serial.print("pushed: /logs/");
  //Serial.println(name);
  
}
void firebaseUpdate()
{
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& object = jsonBuffer.createObject();

  JsonObject& root3 = jsonBuffer.createObject();
  object["hello"] = "world";
  object.printTo(Serial);
  
  root3["nem"] = nem;
  root3["sicaklik"] = sicaklik;
  root3["hava_kalitesi"] = a0read;
  root3["hava_derecesi"] = int(hava_derecesi);
  root3["uv_isini"] = UV_orani;
  root3["tarih"] = tarih;
  root3["saat"] = saat;
  Serial.println("");
  Serial.println("");
  Serial.println("Firebase'e 4 dakikalık güncelleme yapılıyor");
  Serial.println(saat);
  Serial.println(tarih);
  root3.printTo(Serial);

  
  Firebase.setString("anlik/tarih",tarih);
  Firebase.setString("anlik/saat",saat);
  Firebase.setFloat("anlik/nem", nem);
  Firebase.setFloat("anlik/sicaklik", sicaklik);
  Firebase.setFloat("anlik/hava_kalitesi", a0read);
  
  String name = Firebase.push("logs", root3);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  //Serial.print("pushed: /logs/");
  //Serial.println(name);
  
}
void loop()
{

  zamaniGuncelle();
  
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


  //if (millis() - updateTimer > 3600000)
  if (millis() - updateTimer > 120000)
  {
    firebaseUpdate();
    updateTimer = millis();
  };
  if (millis() - updateHalfTimer > 1800000)
  {
    firebaseHalfUpdate();
    updateHalfTimer = millis();
  };

  if (millis() - lastTime > 100000)
  {
    havaDurumu();
    UVDurumu();
    lastTime = millis();
  };
  
  if (millis() - ortamTimer > 5000)
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
