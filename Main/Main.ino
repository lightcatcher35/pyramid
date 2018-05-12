#include <LiquidCrystal.h> // LCD Kütüphanesi
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#include <virtuabotixRTC.h>        // RTC Modülü kütüphanesi 

#include "MQ135.h"                                                                    
 
virtuabotixRTC myRTC(26, 27, 28);

LiquidCrystal lcd(12, 11, 35, 34, 33, 32); // LCD  pinleri
LiquidCrystal lcd2(10, 9, 45, 44, 43, 42); // LCD  pinleri

#define DHT11PIN 36 // DHT11 sensörü pini
const int buttonPin1 = 29;
const int buttonPin2 = 30;
const int buttonPin3 = 31;

dht11 DHT11; // DHT11 nesnesi oluşturuluyor
float sicaklik,nem=0;
float sicaklikGuncel=0;
int mode=0;
int loopCount=0;

float ortSicakliklar[]={0,8.7,9.5,11.6,15.8,22.7,25.5,28.0,27.6,23.6,18.7,14.0,10.4};
float dusukSicakliklar[]={0,5.7,6.1,7.5,11.0,17.3,19.7,22.3,22.2,18.5,14.5,10.6,7.5,13.4 };
float yuksekSicakliklar[]={0,12.3,13.5,16.1,20.8,28.0,30.6,33.1,32.9,29.1,23.9,18.4,14.0,22.6};

String aylar[]={"","Ocak","Subat","Mart","Nisan","Mayis","Haziran","Temmuz","Agustos","Eylul","Ekim","Kasim","Aralik"};


void lcdClear()
{
  lcd.setCursor(0, 0); 
  lcd.print("                 "); 
  lcd.setCursor(0, 1); 
  lcd.print("                 "); 
}
void lcd2Clear()
{
  lcd2.setCursor(0, 0); 
  lcd2.print("                 "); 
  lcd2.setCursor(0, 1); 
  lcd2.print("                 "); 
}

void lcdPrint(String text1,String text2="                 ")
{
  lcdClear();
  lcd.setCursor(0, 0); 
  lcd.print(String(text1));
  lcd.setCursor(0, 1); 
  lcd.print(String(text2));
}
void lcd2Print(String text1,String text2="                 ")
{
  lcd2Clear();
  lcd2.setCursor(0, 0); 
  lcd2.print(String(text1));
  lcd2.setCursor(0, 1); 
  lcd2.print(String(text2));
}
float sicaklikHesapla(int ay,int saat)
{
  float sicaklikY=0;
  Serial.print(saat);
  Serial.print("/");
  Serial.print(ay);
  Serial.println("/");
  if(saat==14)
  {
    sicaklikY=yuksekSicakliklar[ay];
  } 
   else if(saat==4)
  {
    sicaklikY=dusukSicakliklar[ay];
  } else if(saat>4 && saat<14)
  {
    float fark=yuksekSicakliklar[ay]-dusukSicakliklar[ay];
    int saatFark=(saat-4);
    
    //Serial.println("////Fark "+String((fark/9)));
    sicaklikY=dusukSicakliklar[ay]+((fark/9)*saatFark);
    //Serial.println("////////////"+String(sicaklikY));
  }else
  {
    
    float fark=yuksekSicakliklar[ay]-dusukSicakliklar[ay];
    int saatYed=saat;
    //Serial.println("////Saat 1  "+String(saatYed));
    if(saatYed<4) saatYed+=24;
    //Serial.println("////Saat 2  "+String(saatYed));
    saatYed-=14;

    
    sicaklikY=yuksekSicakliklar[ay]-((fark/12)*saatYed);
    //Serial.println("////Saat 3  "+String(saatYed));
    
  }
  return sicaklikY;
}
void setup() {

  //myRTC.setDS1302Time(00, 07, 18, 6, 12, 05, 2018);
  myRTC.updateTime();

  pinMode(buttonPin1, OUTPUT);
  pinMode(buttonPin2, OUTPUT);
  pinMode(buttonPin3, OUTPUT);
  
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd2.begin(16, 2);
  
  lcd.print("Loading");
  delay(2000); 
  lcdClear();
  lcdPrint(String(myRTC.dayofmonth)+" "+String(aylar[myRTC.month])+" "+myRTC.year);
  sicaklikGuncel=sicaklikHesapla(myRTC.month,myRTC.hours);
    
  lcdPrint(String(myRTC.dayofmonth)+" "+String(aylar[myRTC.month])+" "+myRTC.year,"sicaklik : "+String(sicaklikGuncel));;
  delay(2000); 
  lcdClear();
  
}
void loopSlow(){
  
  int chk = DHT11.read(DHT11PIN);
  sicaklik=DHT11.temperature;
  nem=DHT11.humidity;

  Serial.println("okkeeyy");
  if(mode==0)
  {
     lcdPrint(String((String)int(sicaklik))+" C"+" | "+String("Nem %"+(String)int(nem)));
     lcd2Print("Hava Sicakligi","Tahmini "+String(sicaklikGuncel)+" C");
    
  
  }else if(mode==1)
  {
    lcdPrint(String("Sicaklik :"+(String)int(sicaklik)+" C")); 
    lcd2Print("            ");

    if(sicaklik<20)
    {
      lcd2Print("Odadaki sicaklik","fazla dusuk");
    }else if(sicaklik>25)
    { lcd2Print("Odadaki sicaklik","fazla yuksek");
      
    }
    
    
  }else if(mode==2)
  {
    lcdPrint(String("Nem %"+(String)int(nem)));
    String oneriMetin="";
    if(nem>60)
    {
      lcd2Print("Odadaki nem orani","biraz fazla");
    }else if(nem<61 && nem>39)
    {
      lcd2Print("Odadaki nem orani","uygun degerlerde");
    }else
    {
      lcd2Print("Nem orani dusuk.","Hava kuru.");
    }
    lcd.print(oneriMetin);
  }else if(mode==3){
      lcdPrint("Hava Kalitesi (%): ...");
      
      lcd2Print("                  ");
  }
}
void loop() {

  myRTC.updateTime();
  
  loopCount++;
  
  if(loopCount>=1000)
  {
    loopSlow();
    loopCount=0;
  }
 
// Şimdi değerleri okuyalım
//Serial.print("Şuanki Tarih / Saat : ");
//Serial.print(myRTC.dayofmonth);
//Serial.print("/");
//Serial.print(myRTC.month);
//Serial.print("/");
//Serial.print(myRTC.year);
//Serial.print(" ");
//Serial.print(myRTC.hours);
//Serial.print(":");
//Serial.print(myRTC.minutes);
//Serial.print(":");
//Serial.println(myRTC.seconds);

  int button1State = digitalRead(buttonPin1);
  int button2State = digitalRead(buttonPin2);
  int button3State = digitalRead(buttonPin3);

  if(button1State==1)
  {
    mode=1;
    lcdClear();
    loopSlow();
  }else if(button2State==1)
  {
    mode=2;
    lcdClear();
    loopSlow();
  }else if(button3State==1)
  {
    mode=3;
    lcdClear(); 
    loopSlow();
  }

}



