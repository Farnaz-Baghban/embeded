// scalability
// responsiveness
// modular design
// حذف delay()
// instead of delay(2000); we use vTaskDelay(pdMS_TO_TICKS(1000));

// delay کل CPU رو block می‌کند
// vTaskDelay فقط task را suspend می‌کند
// سیستم responsive می‌ماند

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

/* ---------- CONFIG ---------- */

const int gasPin = 34;
const int sampleInterval = 2000;

/* ---------- FREERTOS OBJECTS ---------- */

QueueHandle_t sensorQueue;

/* ---------- SENSOR TASK ---------- */

void sensor_task(void *param)
{
    int gasValue;

    while (1)
    {
        gasValue = analogRead(gasPin);

        Serial.print("Gas reading: ");
        Serial.println(gasValue);

        xQueueSend(sensorQueue, &gasValue, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(sampleInterval));
    }
}

/* ---------- BLUETOOTH TX TASK ---------- */

void bluetooth_tx_task(void *param)
{
    int sensorValue;

    while (1)
    {
        if (xQueueReceive(sensorQueue, &sensorValue, portMAX_DELAY))
        {
            char payload[64];

            sprintf(payload, "{\"device\":\"esp32_sender\",\"gas\":%d}\n", sensorValue);

            SerialBT.print(payload);

            Serial.print("Sent via Bluetooth: ");
            Serial.println(payload);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* ---------- SETUP ---------- */

void setup()
{
    Serial.begin(115200);

    pinMode(gasPin, INPUT);

    SerialBT.begin("ESP32_sender");

    Serial.println("Bluetooth sender started");

    sensorQueue = xQueueCreate(10, sizeof(int));

    xTaskCreatePinnedToCore(
        sensor_task,
        "sensor_task",
        2048,
        NULL,
        1,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        bluetooth_tx_task,
        "bluetooth_tx_task",
        4096,
        NULL,
        1,
        NULL,
        0
    );
}

/* ---------- LOOP ---------- */

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
