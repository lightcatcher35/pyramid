#include <dht11.h>


dht11 DHT11;
void setup();
{
DHT11.attach(2);
Serial.begin(9600);
Serial.println("EMRE KEYKUBAT");
Serial.print("Karşıyaka:");
Serial.println("Ksk");
}
void loop();
{
Serial.println("\n");
int chk = DHT11.read();
Serial.print("Read sensor:");
switch (chk)
  { 
case 0: Serial.println("OK");break;
case -1: Serial.println("hata");break;
case -2: Serial.println("Zaman Asimi");break;
default:Serial.println("Giriş İzni");break;
  }
Serial.print("Nem (%):");
Serial.println((float)DHT11.humidity,DEC);
Serial.print("Nem (°C):");
Serial.println((float)DHT11.temparature,DEC);
Serial.print("Nem (°F):");
Serial.println((float)DHT11.fahrenheit(),DEC);
Serial.print("Nem (°K):");
Serial.println((float)DHT11.kelvin(),DEC);
Serial.print("En Yuksek Deger(°C):");
Serial.println(DHT11.dewPoint(), DEC);
Serial.print("Max Deger(°C):");
Serial.println(DHT11.dewPointFast(), DEC);
delay(2000);
}
