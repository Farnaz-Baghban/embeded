ESP32 IoT Gas Sensor (Bluetooth → MQTT)

Architecture
ESP32 Sensor → Bluetooth → ESP32 Receiver → MQTT Broker

Features
- Bluetooth communication
- MQTT publishing
- FreeRTOS task architecture
- Dynamic network configuration

  
architecture diagram:
Gas Sensor
   │
ESP32 Sender
   │ Bluetooth
ESP32 Receiver
   │
MQTT Broker
   │
Dashboard / Monitoring


