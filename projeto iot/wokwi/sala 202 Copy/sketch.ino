#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <ArduinoJson.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqtt_server = "clusternome-4c6b47d2.a01.euc1.aws.hivemq.cloud";
const int   mqtt_port   = 8883;
const char* mqtt_user   = "rafaccarvalho";
const char* mqtt_pass   = "Objetos1";

const char* topic_pub = "escola/sala202/dados";
const char* topic_sub = "escola/sala202/atuador";

#define DHT_PIN     15
#define LED_R       25
#define LED_G       26
#define LED_B       27
#define BUZZER_PIN  14
#define MQ135_PIN   34

DHTesp dht;
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setRGB(int r, int g, int b) {
  ledcWrite(LED_R, r);
  ledcWrite(LED_G, g);
  ledcWrite(LED_B, b);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  if (msg == "CRITICO") {
    digitalWrite(BUZZER_PIN, HIGH);
    setRGB(255, 0, 0);
  } else if (msg == "ATENCAO") {
    digitalWrite(BUZZER_PIN, LOW);
    setRGB(255, 165, 0);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    setRGB(0, 255, 0);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Sala202", mqtt_user, mqtt_pass)) {
      client.subscribe(topic_sub);
    } else {
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT22);

  ledcAttach(LED_R, 5000, 8);
  ledcAttach(LED_G, 5000, 8);
  ledcAttach(LED_B, 5000, 8);

  pinMode(BUZZER_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi conectado!");

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  TempAndHumidity data = dht.getTempAndHumidity();
  int mq135Raw = analogRead(MQ135_PIN);
  int co2ppm = map(mq135Raw, 0, 4095, 400, 3000);

  StaticJsonDocument<200> doc;
  doc["sala"]      = "Sala 202";
  doc["temp"]      = data.temperature;
  doc["umidade"]   = data.humidity;
  doc["co2"]       = co2ppm;
  doc["timestamp"] = millis();

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(topic_pub, buffer);
  Serial.println(buffer);

  delay(5000);
}