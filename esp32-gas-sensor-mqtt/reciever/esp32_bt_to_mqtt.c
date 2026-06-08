// scalability
// responsiveness
// modular design
// JSON parsing for structured payload
// حذف delay()
// instead of delay(2000); we use vTaskDelay(pdMS_TO_TICKS(1000));

// delay کل CPU رو block می‌کند
// vTaskDelay فقط task را suspend می‌کند
// سیستم responsive می‌ماند

#include "BluetoothSerial.h"
#include <ArduinoJson.h>

BluetoothSerial SerialBT;

/* ---------- FREERTOS OBJECTS ---------- */

QueueHandle_t sensorQueue;

/* ---------- BLUETOOTH RX TASK ---------- */

void bluetooth_rx_task(void *param)
{
    while (1)
    {
        if (SerialBT.available())
        {
            String data = SerialBT.readStringUntil('\n');

            StaticJsonDocument<200> doc;

            DeserializationError error = deserializeJson(doc, data);

            if (!error)
            {
                int gasValue = doc["gas"];

                Serial.print("Received gas value: ");
                Serial.println(gasValue);

                xQueueSend(sensorQueue, &gasValue, portMAX_DELAY);
            }
            else
            {
                Serial.print("JSON parse failed: ");
                Serial.println(error.c_str());
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* ---------- DATA PROCESS TASK ---------- */

void processing_task(void *param)
{
    int value;

    while (1)
    {
        if (xQueueReceive(sensorQueue, &value, portMAX_DELAY))
        {
            Serial.print("Processing gas value: ");
            Serial.println(value);
        }
    }
}

/* ---------- SETUP ---------- */

void setup()
{
    Serial.begin(115200);

    SerialBT.begin("ESP32_receiver", true);

    Serial.println("Connecting to sender...");

    SerialBT.connect("ESP32_sender");

    sensorQueue = xQueueCreate(10, sizeof(int));

    xTaskCreatePinnedToCore(
        bluetooth_rx_task,
        "bluetooth_rx_task",
        4096,
        NULL,
        1,
        NULL,
        0
    );

    xTaskCreatePinnedToCore(
        processing_task,
        "processing_task",
        2048,
        NULL,
        1,
        NULL,
        1
    );
}

/* ---------- LOOP ---------- */

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
