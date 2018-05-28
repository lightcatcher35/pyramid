# Pyramid V1.0


<h1>Akıllı ev asistanı projesi</h1>
<h3>Bu IOT projesinde evdeki ısı, nem, hava kalitesi gibi ölçüp, bu bilgilerle beraber web servisinden alınan hava durumu ve güneş UV derecelerini alıp akıllı karşılaştırmalar yaparak sağlık, izolasyon ve enerji giderleri ile ilgili yorumlar yapan bir IOT cihazı oluşturulması amaçlanmıştır.</h3>
<br>
<h2> Özellik ve Donanımlar</h2>
<p>Cihaz içinde Arduino Wemos D1 board, DHT11 ve MQ135 sensör, potansiyometre, tact butonlar ve 16x2 ldr ekran kullanılmıştır. </p>
<p>Cihazın tasarımı Tinkercad programı ile 3d boyutlu olarak hazırlanarak 3d yazıcıdan çıktı alınarak oluşturulmuştur.</p>
<p>logo tasarımları açık kaynak fontawesome kullanılarak hazırlanmıştır.</p>



<b>nem & sıcaklık</b>

Bebekler için sıcaklık; gündüz 20-24C, gece 1-2 derece daha düşük
Bebekler için nem; %40 - %60 arası

yetişkinler için yazın; 20C 
insan sağlığı için sıcaklık 20-25C 
yetişkinler için nem; %45 - %60

klima değerleri; yazın, 20-21C kışın, 27-28C
burun, sinüzit ve akciğer hastaları uzun süre klimalı ortamda kalmamalı

bir insanın 1 yılda maruz kalacağı radyasyon miktarı 100 milisieverttir. fazlası kansere yol açar.



<b>İhtiyaç Duyulabilecek Linkler</b>

UV ışınlarının etkileri
http://www.wiki-zero.net/index.php?q=aHR0cHM6Ly9lbi53aWtpcGVkaWEub3JnL3dpa2kvVWx0cmF2aW9sZXRfaW5kZXg

Wifi ile konum bulma:
https://github.com/gmag11/WifiLocation

Thingsepak Appinventor bağlantısı
https://www.hackster.io/mjrobot/iot-made-easy-w-uno-esp-01-thingspeak-mit-app-inventor-da6a50

Hava durumunu otomatik alma
https://openweathermap.org/current
