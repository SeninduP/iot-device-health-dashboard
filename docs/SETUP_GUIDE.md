# Complete Setup Guide

## Table of Contents
1. [Raspberry Pi Setup](#raspberry-pi-setup)
2. [ESP32/NodeMCU Setup](#esp32nodemcu-setup)
3. [Testing](#testing)
4. [Troubleshooting](#troubleshooting)

---

## Raspberry Pi Setup

### Prerequisites
- Raspberry Pi Zero W with Raspberry Pi OS installed
- SD card (8GB or larger)
- Laptop/PC with SSH client or terminal
- Same Wi-Fi network as ESP32

### Step 1: Update System

```bash
sudo apt-get update
sudo apt-get upgrade -y
```

### Step 2: Install Python and Pip

```bash
sudo apt-get install -y python3 python3-pip
python3 --version
pip3 --version
```

### Step 3: Clone Repository

```bash
cd ~
git clone https://github.com/SeninduP/iot-device-health-dashboard.git
cd iot-device-health-dashboard
```

### Step 4: Install Python Dependencies

```bash
cd raspberry-pi-dashboard
pip3 install -r requirements.txt
```

### Step 5: Configure API Key (Optional)

Edit `config.py` to change the API key:

```bash
nano config.py
```

Change this line:
```python
API_KEY = os.getenv('API_KEY', 'change-this-secret-key')
```

To:
```python
API_KEY = os.getenv('API_KEY', 'your-secure-api-key')
```

**Important**: Use the same API key in your ESP32 code!

### Step 6: Run Flask Server

```bash
python3 app.py
```

You should see:
```
Starting IoT Device Health Dashboard
API Key: ****-secret-key
Offline timeout: 20 seconds
 * Running on http://0.0.0.0:5000
```

### Step 7: Access Dashboard

From your laptop, open browser and go to:
```
http://<raspberry-pi-ip>:5000
```

Find your Raspberry Pi IP:
```bash
hostname -I
```

### Step 8: Run as Background Service (Optional)

Create a systemd service file:

```bash
sudo nano /etc/systemd/system/iot-dashboard.service
```

Paste:
```ini
[Unit]
Description=IoT Device Health Dashboard
After=network.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/iot-device-health-dashboard/raspberry-pi-dashboard
ExecStart=/usr/bin/python3 /home/pi/iot-device-health-dashboard/raspberry-pi-dashboard/app.py
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
```

Enable service:
```bash
sudo systemctl enable iot-dashboard.service
sudo systemctl start iot-dashboard.service
sudo systemctl status iot-dashboard.service
```

---

## ESP32/NodeMCU Setup

### Prerequisites
- ESP32 or NodeMCU board
- Arduino IDE
- USB cable
- ArduinoJson library (install via Arduino IDE)

### Step 1: Install Arduino IDE

Download from [arduino.cc](https://www.arduino.cc/en/software)

### Step 2: Add Board Support

**For ESP32:**
1. File → Preferences
2. Additional Boards Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Tools → Board → Boards Manager
4. Search "ESP32" → Install "ESP32 by Espressif Systems"

**For NodeMCU:**
1. File → Preferences
2. Additional Boards Manager URLs:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Tools → Board → Boards Manager
4. Search "ESP8266" → Install "esp8266 by ESP8266 Community"

### Step 3: Install ArduinoJson Library

1. Sketch → Include Library → Manage Libraries
2. Search "ArduinoJson"
3. Install "ArduinoJson by Benoit Blanchon"

### Step 4: Upload Code

1. Open `esp32-device/esp32_device_monitor.ino`
2. Configure these settings:

   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* server_ip = "192.168.1.10";  // Your Raspberry Pi IP
   const char* api_key = "change-this-secret-key";
   ```

3. Select board:
   - Tools → Board → ESP32 Dev Module (for ESP32)
   - Tools → Board → NodeMCU 1.0 (for NodeMCU)

4. Select port:
   - Tools → Port → COM3 (Windows) or /dev/ttyUSB0 (Linux/Mac)

5. Upload:
   - Sketch → Upload (or Ctrl+U)

### Step 5: Verify Operation

1. Tools → Serial Monitor
2. Set baud rate to 115200
3. You should see:

```
IoT Device Health Monitor - Starting
[INFO] Connecting to Wi-Fi...
[SUCCESS] Wi-Fi connected!
[INFO] IP Address: 192.168.1.25
[INFO] Signal Strength: -52 dBm
[INFO] Sending device data to server...
[SUCCESS] Data sent successfully!
```

---

## Testing

### Test 1: Verify API Endpoint

```bash
# On your laptop, test the device update endpoint
curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
  -H "Content-Type: application/json" \
  -H "X-API-Key: change-this-secret-key" \
  -d '{
    "device_id": "test-device",
    "device_name": "Test Device",
    "ip_address": "192.168.1.100",
    "wifi_rssi": -55,
    "uptime_seconds": 1000,
    "free_heap": 100000,
    "firmware_version": "1.0.0"
  }'
```

Expected response:
```json
{"status": "success", "message": "Device data updated"}
```

### Test 2: Check Status Endpoint

```bash
curl http://<raspberry-pi-ip>:5000/api/device/status
```

Should return device data with status "online" or "offline".

### Test 3: Dashboard Display

1. Open `http://<raspberry-pi-ip>:5000` in browser
2. Verify all fields are populated
3. Wait 5 seconds to see auto-refresh
4. Turn off ESP32 and verify status changes to "offline" after 20 seconds

---

## Troubleshooting

### Issue: "Cannot connect to Raspberry Pi"

**Check:**
```bash
ping <raspberry-pi-ip>
```

If ping fails:
- Verify both devices on same Wi-Fi network
- Check Raspberry Pi is powered on
- Get Raspberry Pi IP: `hostname -I`

### Issue: "API key authentication failed"

**Solution:**
- Ensure API key matches between ESP32 and Flask app
- Check for extra spaces or typos
- Verify header is exactly `X-API-Key`

### Issue: "Flask server won't start"

**Check:**
```bash
python3 app.py
```

Common errors:
- Port 5000 already in use: `sudo lsof -i :5000`
- Missing dependencies: `pip3 install -r requirements.txt`

### Issue: "ESP32 cannot connect to Wi-Fi"

**Solution:**
- Use 2.4 GHz Wi-Fi (not 5 GHz)
- Check SSID and password
- Move board closer to router
- Check router supports ESP32

---

## Next Steps

1. ✅ Verify both ESP32 and Raspberry Pi communicate
2. ✅ Test dashboard displays live data
3. ✅ Leave running for 24+ hours
4. ✅ Test offline detection
5. Consider future improvements (database, alerts, etc.)
