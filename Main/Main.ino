#include <LiquidCrystal.h> // LCD Kütüphanesi
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // LCD  pinleri

#define DHT11PIN 6 // DHT11 sensörü pini
const int buttonPin1 = 39;
const int buttonPin2 = 40;
const int buttonPin3 = 41;

dht11 DHT11; // DHT11 nesnesi oluşturuluyor
float sicaklik,nem=0;
int mode=0;

void setup() {

  pinMode(buttonPin1, OUTPUT);
  pinMode(buttonPin2, OUTPUT);
  pinMode(buttonPin3, OUTPUT);
  
  Serial.begin(9600); 
  
  lcd.begin(16, 2); 
  lcd.print("Loading");
  delay(2000); 
  lcdClear();
  lcd.setCursor(0, 0); 
  lcd.print("internet ");
  lcd.setCursor(0, 1); 
  lcd.print("bagli degil");
  delay(2000);
  lcdClear();
}
void loop() {

  int button1State = digitalRead(buttonPin1);
  int button2State = digitalRead(buttonPin2);
  int button3State = digitalRead(buttonPin3);

  Serial.println(button1State);

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
    lcd.setCursor(0, 0); 
    lcd.print(String("Nem (%): "+(String)int(nem)));
    lcd.setCursor(0, 1); 
    lcd.print(String("Sicaklik (C):"+(String)int(sicaklik)));
    
  
  }else if(mode==1)
  {
    lcd.setCursor(0, 0); 
    lcd.print(String("Sicaklik (C):"+(String)int(sicaklik)));
    lcd.setCursor(0, 1); 
    lcd.print("               ");
  }else if(mode==2)
  {
    lcd.setCursor(0, 0); 
    lcd.print(String("Nem (%): "+(String)int(nem)));
    lcd.setCursor(0, 1);
    String oneriMetin="";
    if(nem>60)
    {
      oneriMetin="Odadaki nem orani biraz fazla.";
    }else if(nem<61 && nem>39)
    {
      oneriMetin="Nem orani uygun.";
    }
    lcd.print(oneriMetin);
  }else if(mode==3){
    lcd.setCursor(0, 0); 
    lcd.print(String("Hava Kalitesi (%): ..."));
    lcd.setCursor(0, 1); 
    lcd.print("               ");
  }
  
  
  delay(200);
}

void lcdClear()
{
  lcd.setCursor(0, 0); 
  lcd.print("                 "); 
  lcd.setCursor(0, 1); 
  lcd.print("                 "); 
}

