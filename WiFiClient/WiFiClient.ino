
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid     = "paradox35";
const char* password = "SimonSchama35";

const char* host = "api.openweathermap.org";


StaticJsonBuffer<200> jsonBuffer;


void setup() {
  Serial.begin(115200);
  delay(10);

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

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;
void havaDurumu()
{

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/data/2.5/weather?q=Izmir,TR&appid=5485a5c443e83636374329f55e49120d";

  //Serial.print("Requesting URL: ");
  //Serial.println(url);

  // This will send the request to the server
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

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line.indexOf("coord") > 0)
    {

      
      String satir=line;
      //Serial.println(satir);

      StaticJsonBuffer<1000> jsonBuffer;
      JsonObject& root1 = jsonBuffer.parseObject(satir);
      if (root1.success())
      {
        String havaDurumuEn=root1["weather"][0]["main"];
        float havaDerecesi=root1["main"]["temp"];
        havaDerecesi-=273,15;
        Serial.println("Hava Durumu "+String(havaDurumuEn));
        Serial.println(String(havaDerecesi)+" Derece ");
      }
    }

  }


}
void loop() {
  delay(5000);
  ++value;
  havaDurumu();
}

