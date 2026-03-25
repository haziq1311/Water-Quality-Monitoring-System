📌 Project Description

This project is a real-time IoT-based water quality monitoring system designed for aquaculture pond management and environmental monitoring. Built on the ESP32 microcontroller, it continuously measures three critical water parameters — pH, dissolved oxygen (DO), and water temperature — using industrial RS485 Modbus sensors.
Sensor readings are displayed live on a 20x4 I2C LCD screen installed at the site, allowing on-ground operators to check water conditions at a glance. At the same time, data is automatically transmitted to a cloud server in JSON format every 10 minutes using NTP-synchronized scheduling, enabling remote monitoring from anywhere.
The system is designed for continuous, unattended outdoor operation with WiFi auto-reconnect and sensor fault handling (NaN detection) to ensure reliable and consistent data logging. It has been deployed for real aquaculture pond monitoring as part of a live production environment.

🎯 Problem Solved

Manual water quality testing in fish ponds is:

Time-consuming — requires physical presence at the pond
Inconsistent — readings depend on when staff checks
Slow to react — issues like low oxygen levels can kill fish before they are detected

This system solves all three by providing continuous automated monitoring with remote data access and instant visibility into water conditions.

✨ Key Features


📊 Real-time measurement of pH, Dissolved Oxygen (DO), and Temperature

🖥️ Live 20x4 I2C LCD display for on-site readings

🌐 Automated HTTPS cloud data upload every 10 minutes

⏱️ NTP time synchronization for accurate scheduling

📡 Dual RS485 UART channels — dedicated lines for pH/Temp and DO sensors

🔄 WiFi auto-reconnect with LCD status feedback

🛡️ NaN fault detection to prevent bad data from being uploaded




🛠️ Hardware Components

Component Description

-ESP32 Development BoardMain microcontroller

-RS485 to TTL Converter Module (x2)UART1 for pH & Temp sensor

-UART2 for DO sensorpH + Temperature Sensor

-Industrial RS485 Modbus sensorDissolved Oxygen (DO) Sensor

-Industrial RS485 Modbus sensor

-20x4 I2C LCD DisplayOn-site real-time display (I2C address 0x27)

-Power Supply Module12V DC 

-system powerEnclosure BoxWeatherproof housing for outdoor deployment





📊 Data Format (JSON)


json{
  "sensorId":    "YOUR_SENSOR_ID",
  "pondid":       1234567890,
  "pondName":    "YOUR_POND_NAME",
  "temperature":  28.50,
  "pH":           7.20,
  "oxygen":       6.85,
  "voltage":      12.3
}



🚀 Getting Started


1. Install Libraries
In Arduino IDE, install via Library Manager:
ArduinoJson
NTPClient
LiquidCrystal_I2C
UniversalTelegramBot
2. Configure Settings
Open watermonitoring.ino and update the config section at the top:
cppconst char* ssid       = "YOUR_WIFI_SSID";
const char* password   = "YOUR_WIFI_PASSWORD";
const char* serverURL1 = "https://your-server.com/api/sensor-data";
const char* SENSOR_ID  = "YOUR_SENSOR_ID";
const char* POND_NAME  = "YOUR_POND_NAME";
const long  POND_ID    = 0;
3. Upload

Open watermonitoring.ino in Arduino IDE
Select board: ESP32 Dev Module
Select correct COM port
Click Upload


📸 Deployment


This system has been deployed for real aquaculture pond monitoring at a live production site, providing continuous water quality data to support fish farm operations and early detection of water quality issues.

👤 Developer


Muhammad Haziq Bin Md Hanapi
Electronic Engineer — Embedded Systems & IoT
📧 mhaziq1311@gmail.com

📄 License


This project is for portfolio and demonstration purposes.
