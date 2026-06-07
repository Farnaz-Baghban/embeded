#include <BluetoothSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>

BluetoothSerial SerialBT;

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "192.168.1.10";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {

  while (!client.connected()) {

    if (client.connect("ESP32_receiver")) {
      Serial.println("MQTT connected");
    } else {
      delay(2000);
    }

  }
}

void setup() {

  Serial.begin(115200);

  SerialBT.begin("ESP32_receiver", true);

  SerialBT.connect("ESP32_sender");

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  if (SerialBT.available()) {

    String data = SerialBT.readStringUntil('\n');

    Serial.print("Received gas value: ");
    Serial.println(data);

    client.publish("sensor/gas", data.c_str());
  }

}
