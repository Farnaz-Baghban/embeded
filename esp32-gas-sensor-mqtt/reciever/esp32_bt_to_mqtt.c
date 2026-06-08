#include <WiFi.h>
#include <PubSubClient.h>
#include <BluetoothSerial.h>


// why we added FreeRTOS?
// FreeRTOS allows concurrent tasks such as Bluetooth communication and MQTT publishing to run independently 
// without blocking the main execution loop.
//  This improves responsiveness and scalability in IoT systems.

BluetoothSerial SerialBT;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

QueueHandle_t sensorQueue;

String mqttBroker = "";
int mqttPort = 1883;

const char* mqttTopic = "sensor/gas";

/* WiFi credentials (can be changed easily) */
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

/* ---------- WIFI CONNECTION ---------- */

void wifi_connect() {

    Serial.println("Connecting to WiFi...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("Device IP: ");
    Serial.println(WiFi.localIP());
}

/* ---------- MQTT CONNECTION ---------- */

void mqtt_connect() {

    while (!mqttClient.connected()) {

        Serial.println("Connecting to MQTT broker...");

        if (mqttClient.connect("ESP32_receiver")) {
            Serial.println("MQTT connected");
        }
        else {
            Serial.println("MQTT failed, retrying...");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

/* ---------- BLUETOOTH TASK ---------- */

void bluetooth_task(void *param) {

    SerialBT.begin("ESP32_receiver", true);

    Serial.println("Connecting to sender...");

    SerialBT.connect("ESP32_sender");

    while (1) {

        if (SerialBT.available()) {

            String data = SerialBT.readStringUntil('\n');

            int value = data.toInt();

            xQueueSend(sensorQueue, &value, portMAX_DELAY);

        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* ---------- MQTT TASK ---------- */

void mqtt_task(void *param) {

    int sensorValue;

    while (1) {

        if (!mqttClient.connected()) {
            mqtt_connect();
        }

        mqttClient.loop();

        if (xQueueReceive(sensorQueue, &sensorValue, portMAX_DELAY)) {

            char payload[32];
            sprintf(payload, "%d", sensorValue);

            mqttClient.publish(mqttTopic, payload);

            Serial.print("Published: ");
            Serial.println(payload);
        }
    }
}

/* ---------- SETUP ---------- */

void setup() {

    Serial.begin(115200);

    sensorQueue = xQueueCreate(10, sizeof(int));

    wifi_connect();

    /* MQTT broker auto detect example
       here we simply use gateway IP
    */

    IPAddress gateway = WiFi.gatewayIP();
    mqttBroker = gateway.toString();

    mqttClient.setServer(mqttBroker.c_str(), mqttPort);

    xTaskCreatePinnedToCore(
        bluetooth_task,
        "bluetooth_task",
        4096,
        NULL,
        1,
        NULL,
        0
    );

    xTaskCreatePinnedToCore(
        mqtt_task,
        "mqtt_task",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}

/* ---------- LOOP ---------- */

void loop() {

    vTaskDelay(pdMS_TO_TICKS(1000));
}
