#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include "driver/adc.h"
#include "driver/gpio.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ─────────────────────────────────────────────
// CONFIG — Update these before uploading
// ─────────────────────────────────────────────
const char* ssid       = "YOUR_WIFI_SSID";
const char* password   = "YOUR_WIFI_PASSWORD";
const char* serverURL1 = "https://your-server.com/api/sensor-data";

// Sensor / Pond Identity
const char* SENSOR_ID  = "YOUR_SENSOR_ID";
const char* POND_NAME  = "YOUR_POND_NAME";
const long  POND_ID    = 0;  // Replace with your pond ID

// ─────────────────────────────────────────────
// PIN CONFIGURATION
// ─────────────────────────────────────────────
#define I2C_SDA       21   // LCD SDA
#define I2C_SCL       22   // LCD SCL

#define UART1_RX_PIN   4   // RS485 UART1 RX — pH & Temperature
#define UART1_TX_PIN   5   // RS485 UART1 TX — pH & Temperature

#define UART2_RX_PIN  13   // RS485 UART2 RX — Dissolved Oxygen
#define UART2_TX_PIN  12   // RS485 UART2 TX — Dissolved Oxygen

const int uart1 = 15;      // RS485 Direction Control UART1
const int uart2 = 23;      // RS485 Direction Control UART2

// ─────────────────────────────────────────────
// NTP TIME CONFIGURATION
// ─────────────────────────────────────────────
const char* ntpServer             = "pool.ntp.org";
const long  gmtOffsetInSeconds    = 3600;
const int   daylightOffsetInSeconds = 3600;

// ─────────────────────────────────────────────
// OBJECTS
// ─────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 20, 4);
HardwareSerial RS485Serial1(1);
HardwareSerial RS485Serial2(2);
WiFiUDP udp;
NTPClient timeClient(udp, ntpServer, gmtOffsetInSeconds, daylightOffsetInSeconds);

// ─────────────────────────────────────────────
// GLOBAL VARIABLES
// ─────────────────────────────────────────────
String hexString2 = "";
String hexString3 = "";
String hexString4 = "";
String hexString5;
float temperature;
float DOsat;
float DO;
float pH;

// ─────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────
void setup() 
{
  Serial.begin(115200);
  pinMode(uart1, OUTPUT);
  pinMode(uart2, OUTPUT);
  digitalWrite(uart1, LOW);
  digitalWrite(uart2, LOW);
  setup_uart();
  WiFi.begin(ssid, password);
  timeClient.begin();
  timeClient.update();
  delay(1000);
  Serial.println("Setup complete.");
}

// ─────────────────────────────────────────────
// WIFI STATUS DISPLAY
// ─────────────────────────────────────────────
void DisplayWiFiStatus() 
{
    int attempt = 0;
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED && attempt < 5) {
        delay(1000);
        attempt++;
    }

    if (WiFi.status() != WL_CONNECTED) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Not Connected");
        lcd.setCursor(0, 1);
        lcd.print("Proceeding...");
        delay(2000);
        lcd.clear();
    }
}

// ─────────────────────────────────────────────
// MAIN LOOP
// ─────────────────────────────────────────────
void loop() 
{
  Serial.println("loop");
  timeClient.update();
  int second = timeClient.getSeconds();
  int minute = timeClient.getMinutes();
  digitalWrite(uart1, HIGH);
  digitalWrite(uart2, HIGH);

  Serial.println(second);

  LCD();
  drawPage2();
  ph_Func();
  do_Func();

  if (second < 5) 
  {
    transmit();
    switch (minute) 
    {
      case 0:
      case 10:
      case 20:
      case 30:
      case 40:
      case 50:
        Update();
        break;
    }
    delay(200);
  }

  delay(1000);
}

// ─────────────────────────────────────────────
// LCD DISPLAY
// ─────────────────────────────────────────────
void Display() 
{
    lcd.setCursor(0, 0);
    lcd.print("Aquabotics WMS");
    lcd.setCursor(0, 1);
    lcd.print("Temp:");
    lcd.setCursor(0, 2);
    lcd.print("pH:");
    lcd.setCursor(0, 3);
    lcd.print("DO:");
}

void Value()
{
    lcd.setCursor(6, 1);
    lcd.print("       ");
    lcd.setCursor(6, 1);
    lcd.print(temperature, 2);
    lcd.print(" C");

    lcd.setCursor(4, 2);
    lcd.print("       ");
    lcd.setCursor(4, 2);
    lcd.print(pH, 2);

    lcd.setCursor(4, 3);
    lcd.print("       ");
    lcd.setCursor(4, 3);
    lcd.print(DO, 2);
    lcd.print(" mg/L");

    delay(1000);
}

void drawPage2() 
{
    Display(); 
    Value();
}

void LCD() 
{
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(1000);
    lcd.init();
    lcd.backlight();
    DisplayWiFiStatus();
    drawPage2();
}

void Update() 
{
    Value();
    delay(1000);
}

// ─────────────────────────────────────────────
// UART SETUP
// ─────────────────────────────────────────────
void setup_uart() 
{
  RS485Serial1.begin(4800, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  RS485Serial2.begin(4800, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);
}

// ─────────────────────────────────────────────
// pH & TEMPERATURE SENSOR (UART1)
// ─────────────────────────────────────────────
void ph_Func() 
{
  pH = 0;
  transmitHexData("01 03 00 00 00 02 c4 0b");

  delay(500);
  if (RS485Serial1.available()) {
    uint8_t buffer[32];
    size_t bytesRead = RS485Serial1.readBytes(buffer, sizeof(buffer));
    if (bytesRead > 0) {
      processHexData(buffer, bytesRead);
    }
  }
  Serial.print("     pH: ");
  Serial.println(pH, 2);
  Serial.print("     Temperature: ");
  Serial.println(temperature);
}

// ─────────────────────────────────────────────
// DISSOLVED OXYGEN SENSOR (UART2)
// ─────────────────────────────────────────────
void do_Func() 
{
  DOsat = 0;
  DO = 0;
  const char* hexData = "01 03 00 00 00 06 c5 c8";
  uint8_t buffer[32];
  size_t len = hexStringToBytes(hexData, buffer);

  RS485Serial2.write(buffer, len);
  RS485Serial2.flush();
  delay(500);
  if (RS485Serial2.available()) {
    uint8_t buffer[32];
    size_t bytesRead = RS485Serial2.readBytes(buffer, sizeof(buffer));
    if (bytesRead > 0) {
      processHexData2(buffer, bytesRead);
    }
  }
  Serial.print("     DO Concentration: ");
  Serial.println(DO, 2);
}

// ─────────────────────────────────────────────
// DATA PARSING
// ─────────────────────────────────────────────
void processHexData(uint8_t* data, size_t length) 
{
  String hexString = bytesToHexString(data, length);
  hexString2 = hexString;
  hexString3 = hexString;
  Serial.println("Received Hex Data: " + hexString);

  hexString2.remove(0, 6);
  hexString2.remove(4, 8);
  pH = hexToDecimal(hexString2);
  pH = pH / 100;

  hexString3.remove(0, 10);
  hexString3.remove(4, 4);
  temperature = hexToDecimal(hexString3);
  temperature = temperature / 10;
}

void processHexData2(uint8_t* data, size_t length) 
{
  String hexString = bytesToHexString(data, length);
  hexString4 = hexString;
  hexString5 = hexString;
  Serial.println("Received Hex Data: " + hexString);

  hexString5.remove(0, 14);
  hexString5.remove(8, 12);
  Serial.println(hexString5);

  uint32_t hexValue = (uint32_t)strtol(hexString5.c_str(), NULL, 16);

  union {
    uint32_t hex;
    float floatVal;
  } converter;

  converter.hex = hexValue;
  Serial.print(converter.floatVal, 3);
  DO = converter.floatVal;
}

// ─────────────────────────────────────────────
// UTILITY FUNCTIONS
// ─────────────────────────────────────────────
String bytesToHexString(uint8_t* data, size_t length) 
{
  String hexString = "";
  for (size_t i = 0; i < length; i++) {
    if (data[i] < 0x10) hexString += "0";
    hexString += String(data[i], HEX);
  }
  return hexString;
}

int hexToDecimal(const String& hex) 
{
  int decimal = 0;
  for (int i = 0; i < hex.length(); i++) {
    char c = hex.charAt(i);
    int digitValue;
    if (c >= '0' && c <= '9')      digitValue = c - '0';
    else if (c >= 'A' && c <= 'F') digitValue = 10 + (c - 'A');
    else if (c >= 'a' && c <= 'f') digitValue = 10 + (c - 'a');
    else return -1;
    decimal = decimal * 16 + digitValue;
  }
  return decimal;
}

void transmitHexData(const char* hexData) 
{
  uint8_t buffer[32];
  size_t len = hexStringToBytes(hexData, buffer);
  RS485Serial1.write(buffer, len);
  RS485Serial1.flush();
}

size_t hexStringToBytes(const char* hexString, uint8_t* buffer) 
{
  size_t len = strlen(hexString);
  size_t byteCount = 0;
  for (size_t i = 0; i < len; i += 3) {
    char byteStr[3] = { hexString[i], hexString[i + 1], '\0' };
    buffer[byteCount++] = strtoul(byteStr, NULL, 16);
  }
  return byteCount;
}

// ─────────────────────────────────────────────
// DATA TRANSMISSION
// ─────────────────────────────────────────────
void transmit() 
{
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http1;

    DynamicJsonDocument jsonDocument2(200);
    JsonObject root2 = jsonDocument2.to<JsonObject>();

    root2["sensorId"]    = SENSOR_ID;
    root2["pondid"]      = POND_ID;
    root2["pondName"]    = POND_NAME;
    root2["temperature"] = temperature;
    root2["pH"]          = pH;
    root2["voltage"]     = 12.3;

    if (isnan(DO)) DO = 0;
    root2["oxygen"] = DO;

    String jsonString2;
    serializeJson(root2, jsonString2);

    http1.begin(serverURL1);
    http1.addHeader("Content-Type", "application/json");
    http1.setTimeout(10000);
    int httpResponseCode2 = http1.POST(jsonString2);

    if (httpResponseCode2 > 0) {
      if (httpResponseCode2 == HTTP_CODE_OK || httpResponseCode2 == HTTP_CODE_CREATED) {
        Serial.println("Data sent successfully.");
      } else {
        Serial.print("HTTP request failed, code: ");
        Serial.println(httpResponseCode2);
        Serial.println(http1.getString());
      }
    } else {
      Serial.print("Error in HTTP request, code: ");
      Serial.println(httpResponseCode2);
    }

    http1.end();
    jsonDocument2.clear();

  } else {
    Serial.println("WiFi is not connected.");
  }

  delay(300);
}
