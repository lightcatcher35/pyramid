#define ag_ismi "ilkeriphone"
#define ag_sifresi "12345679"
#define IP "184.106.153.149"    //thingspeak.com IP adresi

float sicaklik;

void setup()
{
  Serial.begin(115200); //Seriport'u açıyoruz. Güncellediğimiz
  //ESP modülünün baudRate değeri 115200 olduğu için bizde Seriport'u 115200 şeklinde seçiyoruz

  Serial.println("AT"); //ESP modülümüz ile bağlantı kurulup kurulmadığını kontrol ediyoruz.

  delay(3000); //ESP ile iletişim için 3 saniye bekliyoruz.
  analogReference(INTERNAL);
  if (Serial.find("OK")) {

    
    Serial.println("esp ile bağlanıldı");


  } else
  {
    Serial.println("esp ile bağlantı kurulamadı");
  }
}

void loop() {
}



