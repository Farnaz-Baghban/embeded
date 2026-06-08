ESP32 IoT Gas Sensor (Bluetooth → MQTT)
A modular IoT pipeline for collecting gas sensor data using ESP32 devices and forwarding it to an MQTT broker for monitoring and analysis.

The system uses a two‑device architecture where the first ESP32 reads the sensor and transmits the data via Bluetooth, while the second ESP32 receives the data and forwards it to the MQTT infrastructure.

Architecture
ESP32 Sensor → Bluetooth → ESP32 Receiver → MQTT Broker → Monitoring Dashboard

Features
ESP32-based gas sensor monitoring
Bluetooth communication between devices
MQTT publishing to IoT backend
FreeRTOS task-based architecture
Queue-based inter-task communication
Structured JSON payload for sensor data
Non-blocking design using vTaskDelay
Modular and scalable system design
System Architecture
text
Gas Sensor
   │
ESP32 Sender
   │
   │ Bluetooth (JSON payload)
   │
ESP32 Receiver
   │
   │ MQTT publish
   │
MQTT Broker
   │
Dashboard / Monitoring
Data Format
Sensor data is transmitted using a structured JSON payload.

Example message:

text
{
  "device": "esp32_sender",
  "gas": 742
}
This structure allows easy extension if additional sensors are added later (e.g., temperature or humidity).

FreeRTOS Task Design
Sender device tasks:

Sensor Task

Reads gas sensor values periodically
Pushes readings into a FreeRTOS queue
Bluetooth TX Task

Reads sensor data from the queue
Sends JSON payload over Bluetooth
Receiver device tasks:

Bluetooth RX Task

Receives JSON payload from sender
Parses sensor values
Processing Task

Handles received data
Forwards it to the MQTT pipeline
Why This Architecture
The system is designed to demonstrate:

concurrent embedded programming using FreeRTOS
decoupled task communication using queues
structured IoT data transmission
scalable architecture for future sensors
