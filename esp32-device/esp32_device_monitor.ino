/*
  IoT Device Health & Security Dashboard - ESP32/NodeMCU Firmware
  
  This sketch reads device health information and sends it to a
  Raspberry Pi Flask server every 10 seconds.
  
  Monitored metrics:
  - Wi-Fi signal strength (RSSI)
  - Device uptime
  - Free heap memory
  - Local IP address
  - Firmware version
  
  Communication: HTTP POST with API key authentication
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ============ CONFIGURATION ============

// Wi-Fi Configuration
const char* ssid = "YOUR_WIFI_SSID";           // Change to your Wi-Fi SSID
const char* password = "YOUR_WIFI_PASSWORD";   // Change to your Wi-Fi password

// Server Configuration
const char* server_ip = "192.168.1.10";        // Raspberry Pi IP address
const int server_port = 5000;                   // Flask server port
const char* api_endpoint = "/api/device/update";

// Security Configuration
const char* api_key = "change-this-secret-key"; // Must match Flask API_KEY

// Device Configuration
const char* device_id = "ESP32-Node-01";
const char* device_name = "Living Room ESP32";
const char* firmware_version = "1.0.0";

// Update interval (milliseconds)
const unsigned long UPDATE_INTERVAL = 10000;   // Send data every 10 seconds

// ============ GLOBAL VARIABLES ============

unsigned long last_update = 0;
HTTPClient http;

// ============ SETUP ============

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n");
  Serial.println("========================================");
  Serial.println("IoT Device Health Monitor - Starting");
  Serial.println("========================================\n");
  
  // Connect to Wi-Fi
  connectToWiFi();
}

// ============ MAIN LOOP ============

void loop() {
  // Check if it's time to send data
  if (millis() - last_update >= UPDATE_INTERVAL) {
    last_update = millis();
    
    // Send device data to server
    if (WiFi.status() == WL_CONNECTED) {
      sendDeviceData();
    } else {
      Serial.println("\n[ERROR] Wi-Fi disconnected!");
      connectToWiFi();
    }
  }
  
  delay(100);
}

// ============ FUNCTIONS ============

/**
 * Connect to Wi-Fi network
 */
void connectToWiFi() {
  Serial.println("\n[INFO] Connecting to Wi-Fi...");
  Serial.print("[INFO] SSID: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[SUCCESS] Wi-Fi connected!");
    Serial.print("[INFO] IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("[INFO] Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("[ERROR] Failed to connect to Wi-Fi");
  }
}

/**
 * Read device health metrics
 */
struct DeviceMetrics {
  String ip_address;
  int wifi_rssi;
  unsigned long uptime_seconds;
  uint32_t free_heap;
};

DeviceMetrics getDeviceMetrics() {
  DeviceMetrics metrics;
  
  // Get IP address
  metrics.ip_address = WiFi.localIP().toString();
  
  // Get Wi-Fi signal strength (RSSI)
  metrics.wifi_rssi = WiFi.RSSI();
  
  // Get uptime in seconds
  metrics.uptime_seconds = millis() / 1000;
  
  // Get free heap memory
  metrics.free_heap = ESP.getFreeHeap();
  
  return metrics;
}

/**
 * Send device data to Raspberry Pi server
 */
void sendDeviceData() {
  Serial.println("\n[INFO] Sending device data to server...");
  Serial.print("[INFO] Server: ");
  Serial.print(server_ip);
  Serial.print(":");
  Serial.println(server_port);
  
  // Get current device metrics
  DeviceMetrics metrics = getDeviceMetrics();
  
  // Create JSON payload
  StaticJsonDocument<512> doc;
  doc["device_id"] = device_id;
  doc["device_name"] = device_name;
  doc["ip_address"] = metrics.ip_address;
  doc["wifi_rssi"] = metrics.wifi_rssi;
  doc["uptime_seconds"] = metrics.uptime_seconds;
  doc["free_heap"] = metrics.free_heap;
  doc["firmware_version"] = firmware_version;
  
  // Serialize JSON to string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Construct URL
  String url = "http://";
  url += server_ip;
  url += ":";
  url += server_port;
  url += api_endpoint;
  
  // Perform HTTP POST request
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", api_key);
  
  int httpResponseCode = http.POST(jsonString);
  
  // Handle response
  if (httpResponseCode > 0) {
    Serial.print("[HTTP] Response Code: ");
    Serial.println(httpResponseCode);
    
    if (httpResponseCode == 200) {
      Serial.println("[SUCCESS] Data sent successfully!");
      String response = http.getString();
      Serial.print("[INFO] Response: ");
      Serial.println(response);
    } else if (httpResponseCode == 401) {
      Serial.println("[ERROR] Unauthorized - Invalid API key!");
    } else {
      Serial.println("[ERROR] HTTP request failed");
    }
  } else {
    Serial.print("[ERROR] HTTP request failed: ");
    Serial.println(http.errorToString(httpResponseCode).c_str());
  }
  
  http.end();
  
  // Print current metrics for debugging
  Serial.println("\n[DEBUG] Device Metrics:");
  Serial.print("  IP Address: ");
  Serial.println(metrics.ip_address);
  Serial.print("  Wi-Fi RSSI: ");
  Serial.print(metrics.wifi_rssi);
  Serial.println(" dBm");
  Serial.print("  Uptime: ");
  Serial.print(metrics.uptime_seconds);
  Serial.println(" seconds");
  Serial.print("  Free Heap: ");
  Serial.print(metrics.free_heap);
  Serial.println(" bytes");
  Serial.println();
}
