#include <LiquidCrystal.h> // LCD Kütüphanesi
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.
#include <virtuabotixRTC.h>        // RTC Modülü kütüphanesi                                                                     
 
virtuabotixRTC myRTC(28, 29, 30);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // LCD  pinleri
LiquidCrystal lcd2(10, 9, 25, 24, 23, 22); // LCD  pinleri

#define DHT11PIN 6 // DHT11 sensörü pini
const int buttonPin1 = 39;
const int buttonPin2 = 40;
const int buttonPin3 = 41;

dht11 DHT11; // DHT11 nesnesi oluşturuluyor
float sicaklik,nem=0;
int mode=0;

float ortSicakliklar[]={0,8.7,9.5,11.6,15.8,20.7,25.5,28.0,27.6,23.6,18.7,14.0,10.4};
float dusukSicakliklar[]={0,5.7,6.1,7.5,11.0,15.3,19.7,22.3,22.2,18.5,14.5,10.6,7.5,13.4 };
float yuksekSicakliklar[]={0,12.3,13.5,16.1,20.8,26.0,30.6,33.1,32.9,29.1,23.9,18.4,14.0,22.6};
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
void setup() {

  pinMode(buttonPin1, OUTPUT);
  pinMode(buttonPin2, OUTPUT);
  pinMode(buttonPin3, OUTPUT);
  
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  lcd2.begin(16, 2);
  
  lcd.print("Loading");
  delay(2000); 
  lcdClear();
  lcdPrint(String(ortSicakliklar[1]));
  delay(2000); 
  lcdClear();
  
}
void loop() {

  myRTC.updateTime();
 
// Şimdi değerleri okuyalım
Serial.print("Şuanki Tarih / Saat : ");
Serial.print(myRTC.dayofmonth);
Serial.print("/");
Serial.print(myRTC.month);
Serial.print("/");
Serial.print(myRTC.year);
Serial.print(" ");
Serial.print(myRTC.hours);
Serial.print(":");
Serial.print(myRTC.minutes);
Serial.print(":");
Serial.println(myRTC.seconds);

  int button1State = digitalRead(buttonPin1);
  int button2State = digitalRead(buttonPin2);
  int button3State = digitalRead(buttonPin3);


  if(button1State==1)
  {
    mode=1;
    lcdClear();
    
  }else if(button2State==1)
  {
    mode=2;
    lcdClear();
    
  }else if(button3State==1)
  {
    mode=3;
    lcdClear();
    
  }

  
  
  int chk = DHT11.read(DHT11PIN);
  sicaklik=DHT11.temperature;
  nem=DHT11.humidity;

  if(mode==0)
  {
     lcdPrint(String("Nem %"+(String)int(nem)),String("Sicaklik (C):"+(String)int(sicaklik)));
    
  
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
  
  
  delay(200);
}



