# ESP32/NodeMCU Device Setup Guide

This guide explains how to set up the ESP32 or NodeMCU ESP8266 device to send health data to the Raspberry Pi dashboard server.

---

## 📋 Prerequisites

- ESP32 board or NodeMCU ESP8266
- Arduino IDE installed on your laptop/PC
- USB cable to connect the board to your computer
- Wi-Fi network (2.4 GHz recommended)
- Raspberry Pi IP address (e.g., `192.168.1.10`)

---

## 🔧 Step 1: Install Arduino IDE

### Windows/Mac/Linux
1. Download from [arduino.cc](https://www.arduino.cc/en/software)
2. Install the IDE
3. Launch Arduino IDE

---

## 📦 Step 2: Add ESP32 Board Support

### For ESP32:

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. In "Additional Boards Manager URLs", paste:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click **OK**
5. Go to **Tools** → **Board** → **Boards Manager**
6. Search for "ESP32"
7. Install "ESP32 by Espressif Systems"
8. Wait for installation to complete

### For NodeMCU ESP8266:

1. Open Arduino IDE
2. Go to **File** → **Preferences**
3. In "Additional Boards Manager URLs", paste:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Click **OK**
5. Go to **Tools** → **Board** → **Boards Manager**
6. Search for "ESP8266"
7. Install "esp8266 by ESP8266 Community"
8. Wait for installation to complete

---

## 🔌 Step 3: Connect Board to Computer

1. Connect ESP32/NodeMCU to your laptop via USB
2. Open Arduino IDE
3. Go to **Tools** → **Port**
4. Select the COM port (e.g., COM3, /dev/ttyUSB0)
5. Go to **Tools** → **Board**
   - For ESP32: Select "ESP32 Dev Module"
   - For NodeMCU: Select "NodeMCU 1.0 (ESP8266-12E Module)"

---

## 📝 Step 4: Configure Your Code

Open `esp32_device_monitor.ino` and update these settings:

### Wi-Fi Credentials
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### Raspberry Pi Server Address
```cpp
const char* server_ip = "192.168.1.10";  // Change to your Raspberry Pi IP
const int server_port = 5000;             // Flask default port
```

### API Key (Must match Flask server)
```cpp
const char* api_key = "change-this-secret-key";
```

### Device Information (Optional)
```cpp
const char* device_id = "ESP32-Node-01";
const char* device_name = "Living Room ESP32";
const char* firmware_version = "1.0.0";
```

---

## ⬆️ Step 5: Upload Code to Board

1. Click **Sketch** → **Upload** (or press Ctrl+U)
2. Wait for compilation and upload
3. You should see: "Leaving... Hard resetting via RTS pin..."
4. Upload complete!

---

## 🔍 Step 6: Verify in Serial Monitor

1. Go to **Tools** → **Serial Monitor**
2. Set baud rate to **115200**
3. You should see output like:

```
Starting device...
Connecting to Wi-Fi: MyWiFi
Wi-Fi connected!
IP Address: 192.168.1.25
Sending data to: 192.168.1.10:5000
Response: {"status":"success","message":"Device data updated"}
Data sent successfully!
```

---

## 🚨 Troubleshooting

### Issue: "Port not recognized" or "No COM port available"
**Solution:**
- Install USB driver for your board
- Try a different USB cable
- Restart Arduino IDE

### Issue: "Board not detected"
**Solution:**
- Check you selected the correct board in Tools → Board
- Press the BOOT button on the board during upload

### Issue: "Connection refused" to Raspberry Pi
**Solution:**
- Verify both devices are on the same Wi-Fi network
- Check Raspberry Pi IP address using `hostname -I`
- Verify Flask server is running on Raspberry Pi
- Check API key matches between ESP32 and Flask app

### Issue: "Wi-Fi connection fails"
**Solution:**
- Use 2.4 GHz Wi-Fi (not 5 GHz)
- Check SSID and password are correct
- Move board closer to router
- Restart the board

---

## 📊 Data Being Sent

Every 10 seconds, the ESP32 sends this JSON to the server:

```json
{
  "device_id": "ESP32-Node-01",
  "device_name": "Living Room ESP32",
  "ip_address": "192.168.1.25",
  "wifi_rssi": -52,
  "uptime_seconds": 3560,
  "free_heap": 187432,
  "firmware_version": "1.0.0"
}
```

---

## ✅ Success Indicators

✓ Serial Monitor shows "Wi-Fi connected!"  
✓ Serial Monitor shows your local IP address  
✓ Serial Monitor shows successful responses from server  
✓ Dashboard on Raspberry Pi shows device status as "Online"  
✓ Dashboard updates every 5 seconds with fresh data  

---

## 📚 Next Steps

1. Set up Raspberry Pi Flask server (see `docs/SETUP_GUIDE.md`)
2. Access the web dashboard
3. Verify device appears as "Online"
4. Leave the ESP32 running for 24+ hours to test offline detection

---

**Troubleshooting Help**: See main repository `docs/TROUBLESHOOTING.md`