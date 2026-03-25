# 💧 Water Quality Monitoring System (IoT)

A real-time IoT-based water quality monitoring system built on ESP32, measuring pH, dissolved oxygen (DO), and temperature via RS485 sensors. Data is transmitted to a remote server via HTTPS and displayed on a 20x4 I2C LCD, with periodic updates every 10 minutes.

---

## 📌 Overview

Manual water quality testing is time-consuming and often delayed. This system provides continuous, automated monitoring of key water parameters — displaying live readings on a local LCD and transmitting data remotely to a cloud server at regular intervals.

---

## ✨ Features

- 📊 **Real-time monitoring** of:
  - pH level
  - Dissolved Oxygen (DO) in mg/L
  - Water Temperature in °C
- 🖥️ **20x4 I2C LCD display** for on-site live readings
- 🌐 **Remote HTTPS data transmission** to cloud server (JSON format)
- ⏱️ **Scheduled uploads every 10 minutes** using NTP time sync
- 📡 **Dual RS485 UART channels** — separate serial lines for pH/Temp and DO sensors
- 🔄 **WiFi auto-reconnect** with status feedback on LCD

---

## 🛠️ Tech Stack

| Component | Technology |
|---|---|
| Microcontroller | ESP32 |
| Sensor Communication | RS485 (Modbus RTU) |
| Display | I2C LCD 20x4 (address 0x27) |
| Data Transmission | HTTPS / HTTP POST (JSON) |
| Time Sync | NTP Client (pool.ntp.org) |
| Programming Language | C/C++ (Arduino Framework) |

---

## 🔌 Hardware Components

| Component | Details |
|---|---|
| ESP32 Development Board | Main controller |
| RS485 to TTL Module (x2) | UART1 for pH & Temp, UART2 for DO |
| pH + Temperature Sensor | RS485 Modbus, connected to UART1 (RX:4, TX:5) |
| Dissolved Oxygen (DO) Sensor | RS485 Modbus, connected to UART2 (RX:13, TX:12) |
| I2C LCD 20x4 | SDA: GPIO21, SCL: GPIO22 |
| RS485 Direction Control Pins | UART1: GPIO15, UART2: GPIO23 |

---

## 📁 Project Structure

```
water-quality-monitor/
│
├── main.ino               # Main sketch — setup, loop, LCD, WiFi, scheduling
└── README.md
```

---

## ⚙️ How It Works

```
RS485 Sensors
  pH + Temp (UART1)    DO (UART2)
        ↓                  ↓
     ESP32 reads Modbus RTU data
              ↓
       Parse hex response
       → pH / Temp / DO values
              ↓
      Display on I2C LCD (20x4)
              ↓
      Every second < 5 of each minute:
         POST JSON to HTTPS server
              ↓
      Every 10 minutes:
         Update LCD values
```

---

## 🚀 Getting Started

### Requirements

Install these libraries in Arduino IDE:

```
WiFi                  (built-in ESP32)
HTTPClient            (built-in ESP32)
ArduinoJson
NTPClient
WiFiUdp
WiFiClientSecure
UniversalTelegramBot
LiquidCrystal_I2C
HardwareSerial        (built-in ESP32)
```

### Configuration

Before uploading, update these values in `main.ino`:

```cpp
// WiFi credentials
const char* ssid = "your-wifi-name";
const char* password = "your-wifi-password";

// Server endpoint
const char* serverURL1 = "https://your-server.com/api/sensor-data";

// Pond / Sensor identity
root2["sensorId"]  = "YOUR_SENSOR_ID";
root2["pondid"]    = YOUR_POND_ID;
root2["pondName"]  = "YOUR_POND_NAME";
```

### Pin Configuration

```cpp
// I2C LCD
#define I2C_SDA 21
#define I2C_SCL 22

// RS485 UART1 — pH & Temperature
#define UART1_RX_PIN 4
#define UART1_TX_PIN 5

// RS485 UART2 — Dissolved Oxygen
#define UART2_RX_PIN 13
#define UART2_TX_PIN 12
```

### Upload

1. Open `main.ino` in Arduino IDE
2. Select board: **ESP32 Dev Module**
3. Select correct COM port
4. Click **Upload**

---

## 📊 Sample JSON Sent to Server

```json
{
  "sensorId": "YOUR_SENSOR_ID",
  "oxygen": 6.85,
  "temperature": 28.50,
  "pH": 7.20,
  "voltage": 12.3,
  "pondid": 1234567890,
  "pondName": "YOUR_POND_NAME"
}
```

---

## ⏱️ Data Transmission Schedule

Data is sent to the server when the NTP second count is below 5, and LCD values are refreshed every **10 minutes** (at minutes 0, 10, 20, 30, 40, 50).

---

## ⚠️ Notes

- Make sure your RS485 sensors use **Modbus RTU** protocol at **4800 baud**
- The DO sensor response is parsed as a **32-bit IEEE 754 float**
- pH and Temperature are parsed as **16-bit integers** divided by 100 and 10 respectively

---

## 👤 Developer

**Muhammad Haziq Bin Md Hanapi**
Electronic Engineer — Embedded Systems & IoT
📧 mhaziq1311@gmail.com

---

## 📄 License

This project is for portfolio and demonstration purposes.
