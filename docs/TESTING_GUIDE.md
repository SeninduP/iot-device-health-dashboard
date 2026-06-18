# Complete Testing Guide

This guide walks you through testing the entire IoT Device Health & Security Dashboard project from start to finish.

---

## 📋 Table of Contents

1. [Pre-Testing Checklist](#pre-testing-checklist)
2. [Phase 1: Hardware Setup Testing](#phase-1-hardware-setup-testing)
3. [Phase 2: Raspberry Pi Server Testing](#phase-2-raspberry-pi-server-testing)
4. [Phase 3: ESP32/NodeMCU Device Testing](#phase-3-esp32nodemcu-device-testing)
5. [Phase 4: Integration Testing](#phase-4-integration-testing)
6. [Phase 5: Dashboard UI Testing](#phase-5-dashboard-ui-testing)
7. [Phase 6: Security Testing](#phase-6-security-testing)
8. [Phase 7: Long-Term Reliability Testing](#phase-7-long-term-reliability-testing)
9. [Troubleshooting](#troubleshooting)

---

## ✅ Pre-Testing Checklist

Before you start testing, verify you have:

- [ ] **Hardware**
  - [ ] ESP32 or NodeMCU board
  - [ ] Raspberry Pi Zero W with microSD card
  - [ ] USB cables for power
  - [ ] Both devices on same Wi-Fi network
  - [ ] Laptop/PC for development

- [ ] **Software**
  - [ ] Arduino IDE installed
  - [ ] Python 3 installed on Raspberry Pi
  - [ ] Git installed
  - [ ] GitHub repository cloned locally

- [ ] **Network**
  - [ ] Both devices connected to 2.4 GHz Wi-Fi
  - [ ] Devices can ping each other
  - [ ] No firewall blocking port 5000

- [ ] **Credentials**
  - [ ] Wi-Fi SSID and password
  - [ ] Raspberry Pi IP address
  - [ ] ESP32/NodeMCU board type

---

## 🔧 Phase 1: Hardware Setup Testing

### Test 1.1: Verify Raspberry Pi Connection

**Objective**: Ensure Raspberry Pi is accessible on the network

**Steps**:
1. Power on Raspberry Pi
2. Wait 60 seconds for boot
3. Find Raspberry Pi IP address:
   ```bash
   # On Raspberry Pi terminal
   hostname -I
   ```
4. From your laptop, test ping:
   ```bash
   ping <raspberry-pi-ip>
   # Example: ping 192.168.1.10
   ```

**Expected Result**:
```
Pinging 192.168.1.10 with 32 bytes of data:
Reply from 192.168.1.10: bytes=32 time=25ms TTL=64
Reply from 192.168.1.10: bytes=32 time=22ms TTL=64
```

**If Failed**:
- Check Raspberry Pi is powered on (red LED should be on)
- Verify both devices on same Wi-Fi network
- Try restarting Raspberry Pi

---

### Test 1.2: Verify ESP32/NodeMCU Connection

**Objective**: Ensure ESP32 is accessible on the network

**Steps**:
1. Power on ESP32/NodeMCU board
2. Wait 5 seconds for boot
3. From your laptop, test ping:
   ```bash
   ping 192.168.1.25
   # (or whatever IP is assigned to ESP32)
   ```

**Expected Result**:
```
Pinging 192.168.1.25 with 32 bytes of data:
Reply from 192.168.1.25: bytes=32 time=15ms TTL=64
```

**If Failed**:
- Check board is powered (LED should blink or be lit)
- Verify same 2.4 GHz Wi-Fi network
- Check Arduino code has correct SSID/password

---

### Test 1.3: Verify Network Connectivity Between Devices

**Objective**: Ensure Raspberry Pi and ESP32 can communicate

**Steps**:
1. SSH into Raspberry Pi:
   ```bash
   ssh pi@<raspberry-pi-ip>
   # Or use PuTTY on Windows
   ```

2. From Raspberry Pi, ping ESP32:
   ```bash
   ping 192.168.1.25
   ```

**Expected Result**:
```
PING 192.168.1.25 (192.168.1.25) 56(84) bytes of data.
64 bytes from 192.168.1.25: icmp_seq=1 ttl=64 time=12.3 ms
64 bytes from 192.168.1.25: icmp_seq=1 ttl=64 time=11.8 ms
```

**✅ Checkpoint**: All devices can reach each other on network

---

## 🐍 Phase 2: Raspberry Pi Server Testing

### Test 2.1: Install Dependencies

**Objective**: Ensure Python dependencies are installed correctly

**Steps**:
1. SSH into Raspberry Pi
2. Navigate to project:
   ```bash
   cd ~/iot-device-health-dashboard/raspberry-pi-dashboard
   ```

3. Install dependencies:
   ```bash
   pip3 install -r requirements.txt
   ```

4. Verify Flask is installed:
   ```bash
   python3 -c "import flask; print(flask.__version__)"
   ```

**Expected Result**:
```
Collecting Flask==2.3.3
...
Successfully installed Flask-2.3.3 Werkzeug-2.3.7
```

And when checking version:
```
2.3.3
```

**If Failed**:
```bash
# Try upgrading pip
pip3 install --upgrade pip
# Then reinstall
pip3 install -r requirements.txt
```

---

### Test 2.2: Start Flask Server

**Objective**: Verify Flask server starts without errors

**Steps**:
1. On Raspberry Pi, start the server:
   ```bash
   python3 app.py
   ```

2. Watch the output carefully

**Expected Result**:
```
Starting IoT Device Health Dashboard
API Key: ****-secret-key
Offline timeout: 20 seconds
 * Running on http://0.0.0.0:5000
 * Press CTRL+C to quit
```

**If Failed**:
- Check for port conflicts: `sudo lsof -i :5000`
- Check for Python errors in output
- Ensure all files are in correct directory

**Keep this terminal open** - the server needs to stay running for next tests.

---

### Test 2.3: Test API Health Check Endpoint

**Objective**: Verify Flask API is responding

**Steps**:
1. Open a NEW terminal on your laptop (keep Flask running in previous terminal)
2. Test health endpoint:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/health
   ```

**Expected Result**:
```json
{
  "status": "ok",
  "timestamp": "2026-06-18T15:30:45.123456",
  "api_version": "1.0.0"
}
```

**If Failed**:
- Ensure Flask server is still running
- Check Raspberry Pi IP is correct
- Verify firewall isn't blocking port 5000
- Check Flask output for errors

---

### Test 2.4: Test Device Update Endpoint (Invalid API Key)

**Objective**: Verify API key validation is working

**Steps**:
1. From your laptop, send data with WRONG API key:
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -H "X-API-Key: wrong-api-key" \
     -d '{
       "device_id": "ESP32-Node-01",
       "device_name": "Test Device",
       "ip_address": "192.168.1.25",
       "wifi_rssi": -52,
       "uptime_seconds": 1000,
       "free_heap": 187432,
       "firmware_version": "1.0.0"
     }'
   ```

**Expected Result**:
```json
{
  "status": "error",
  "message": "Invalid API key"
}
```

HTTP Status: **401 Unauthorized**

**✅ Security check passed**: Wrong API key rejected

---

### Test 2.5: Test Device Update Endpoint (Valid API Key)

**Objective**: Verify device data is accepted with correct API key

**Steps**:
1. Send data with CORRECT API key:
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -H "X-API-Key: change-this-secret-key" \
     -d '{
       "device_id": "ESP32-Node-01",
       "device_name": "Test Device",
       "ip_address": "192.168.1.25",
       "wifi_rssi": -52,
       "uptime_seconds": 1000,
       "free_heap": 187432,
       "firmware_version": "1.0.0"
     }'
   ```

**Expected Result**:
```json
{
  "status": "success",
  "message": "Device data updated",
  "timestamp": "2026-06-18T15:30:50.654321"
}
```

HTTP Status: **200 OK**

**✅ Device update endpoint working**

---

### Test 2.6: Test Device Status Endpoint

**Objective**: Verify status endpoint returns device data

**Steps**:
1. After sending device data (from Test 2.5), immediately request status:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/device/status
   ```

**Expected Result**:
```json
{
  "device_id": "ESP32-Node-01",
  "device_name": "Test Device",
  "ip_address": "192.168.1.25",
  "wifi_rssi": -52,
  "uptime_seconds": 1000,
  "free_heap": 187432,
  "firmware_version": "1.0.0",
  "status": "online",
  "last_seen": "2026-06-18T15:30:50.654321",
  "time_since_update": 0
}
```

Notice:
- `"status": "online"` (device is online)
- `"last_seen"` has the timestamp
- `"time_since_update"` shows seconds since last update

**✅ Status endpoint working**

---

### Test 2.7: Test Offline Detection

**Objective**: Verify device goes offline after 20 seconds

**Steps**:
1. Send device data (Test 2.5):
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -H "X-API-Key: change-this-secret-key" \
     -d '{"device_id": "ESP32-Node-01", "device_name": "Test Device", "ip_address": "192.168.1.25", "wifi_rssi": -52, "uptime_seconds": 1000, "free_heap": 187432, "firmware_version": "1.0.0"}'
   ```

2. Check status immediately:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/device/status
   ```
   
   Should show: `"status": "online"`

3. Wait 21 seconds (do NOT send any more data)

4. Check status again:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/device/status
   ```

**Expected Result** (after 21 seconds):
```json
{
  ...
  "status": "offline",
  "last_seen": "2026-06-18T15:30:50.654321",
  "time_since_update": 21
}
```

Notice:
- `"status": "offline"` (device hasn't sent data for 21 seconds)
- `"time_since_update": 21`

**✅ Offline detection working correctly**

---

## ✅ Phase 2 Checkpoint

- [ ] Flask server starts successfully
- [ ] Health endpoint responds
- [ ] Invalid API key rejected (401)
- [ ] Valid API key accepted (200)
- [ ] Device data stored correctly
- [ ] Status endpoint returns correct data
- [ ] Offline detection works (after 20 seconds)

**Keep Flask server running for Phase 3**

---

## 🎮 Phase 3: ESP32/NodeMCU Device Testing

### Test 3.1: Prepare Arduino Code

**Objective**: Configure ESP32 code with correct credentials

**Steps**:
1. Open `esp32-device/esp32_device_monitor.ino` in Arduino IDE

2. Update these lines:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";           // Your Wi-Fi name
   const char* password = "YOUR_WIFI_PASSWORD";   // Your Wi-Fi password
   const char* server_ip = "192.168.1.10";        // Raspberry Pi IP
   const char* api_key = "change-this-secret-key"; // Must match Flask
   ```

3. Save the file

**✅ Configuration complete**

---

### Test 3.2: Upload Code to ESP32

**Objective**: Successfully upload firmware to board

**Steps**:
1. Connect ESP32 to laptop via USB cable
2. In Arduino IDE, select board:
   - Tools → Board → ESP32 Dev Module (for ESP32)
   - Tools → Board → NodeMCU 1.0 (for NodeMCU)

3. Select port:
   - Tools → Port → COM3 (or the port showing your board)

4. Click Upload (or Sketch → Upload)

5. Watch the output:
   ```
   Compiling sketch...
   Uploading to COM3...
   ...
   Leaving... Hard resetting via RTS pin...
   ```

**Expected Result**:
```
Upload complete!
```

**If Failed**:
- Try pressing BOOT button on board during upload
- Check USB drivers are installed
- Try different USB cable

---

### Test 3.3: Monitor Serial Output

**Objective**: Verify ESP32 connects to Wi-Fi and sends data

**Steps**:
1. Click Tools → Serial Monitor
2. Set baud rate to **115200** (bottom right corner)
3. Watch the output for 30 seconds

**Expected Output**:
```
========================================
IoT Device Health Monitor - Starting
========================================

[INFO] Connecting to Wi-Fi...
[INFO] SSID: MyWiFi
..........
[SUCCESS] Wi-Fi connected!
[INFO] IP Address: 192.168.1.25
[INFO] Signal Strength: -52 dBm

[INFO] Sending device data to server...
[INFO] Server: 192.168.1.10:5000
[HTTP] Response Code: 200
[SUCCESS] Data sent successfully!
[INFO] Response: {"status":"success","message":"Device data updated",...}

[DEBUG] Device Metrics:
  IP Address: 192.168.1.25
  Wi-Fi RSSI: -52 dBm
  Uptime: 5 seconds
  Free Heap: 187432 bytes

[INFO] Sending device data to server...
[HTTP] Response Code: 200
[SUCCESS] Data sent successfully!
...
```

**Key indicators**:
- ✅ "Wi-Fi connected!"
- ✅ IP address displayed
- ✅ "Response Code: 200"
- ✅ "Data sent successfully!"
- ✅ Data appears every 10 seconds

**If Failed - No Wi-Fi Connection**:
```
[INFO] Connecting to Wi-Fi...
.................
[ERROR] Failed to connect to Wi-Fi
```

Solutions:
- Check SSID and password are correct (no spaces, case-sensitive)
- Ensure 2.4 GHz Wi-Fi (not 5 GHz)
- Move board closer to router
- Try resetting board (press RESET button)

**If Failed - Connection Refused**:
```
[INFO] Sending device data to server...
[ERROR] HTTP request failed: Connection refused
```

Solutions:
- Check Raspberry Pi IP is correct
- Verify Flask server is running
- Check both on same Wi-Fi network
- Verify port 5000 is open

**If Failed - Invalid API Key**:
```
[HTTP] Response Code: 401
[ERROR] Unauthorized - Invalid API key!
```

Solutions:
- Ensure `api_key` in Arduino code matches Flask `API_KEY`
- Check for extra spaces or typos
- Restart ESP32 after changing code

---

### Test 3.4: Verify Data in Raspberry Pi Server

**Objective**: Confirm ESP32 data is being received by Raspberry Pi

**Steps**:
1. Check Flask server terminal (should show activity):
   ```
   2026-06-18 15:30:50.654 - app - INFO - Device data updated from ESP32-Node-01
   ```

2. From your laptop, check status endpoint:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/device/status
   ```

3. Verify the response shows ESP32 data:
   ```json
   {
     "device_id": "ESP32-Node-01",
     "device_name": "Living Room ESP32",
     "ip_address": "192.168.1.25",
     "wifi_rssi": -52,
     "uptime_seconds": 23,
     "free_heap": 187432,
     "firmware_version": "1.0.0",
     "status": "online",
     "last_seen": "2026-06-18T15:30:50.654321"
   }
   ```

**✅ Device connected and sending data successfully**

---

## 📊 Phase 4: Integration Testing

### Test 4.1: Verify Continuous Data Flow

**Objective**: Ensure ESP32 continuously sends data every 10 seconds

**Steps**:
1. Monitor Serial output for 60 seconds
2. Count successful sends (should be ~6 sends in 60 seconds)
3. Check timestamp differences

**Expected Pattern**:
```
[TIME 0s] Data sent successfully!
[TIME 10s] Data sent successfully!
[TIME 20s] Data sent successfully!
[TIME 30s] Data sent successfully!
[TIME 40s] Data sent successfully!
[TIME 50s] Data sent successfully!
[TIME 60s] Data sent successfully!
```

**Verify on server side**:
```bash
# Run this every 5 seconds
curl http://<raspberry-pi-ip>:5000/api/device/status | grep uptime_seconds
```

The uptime should continuously increase:
```
"uptime_seconds": 23
"uptime_seconds": 33
"uptime_seconds": 43
"uptime_seconds": 53
```

**✅ Continuous data flow verified**

---

### Test 4.2: Verify Data Accuracy

**Objective**: Ensure device metrics are accurate

**Steps**:
1. Note ESP32 Serial Monitor uptime (e.g., "Uptime: 120 seconds")
2. Check server status endpoint
3. Compare values

**Expected Result**:
```
ESP32 Serial Monitor shows: "Uptime: 120 seconds"
Server endpoint shows: "uptime_seconds": 120
→ Values match ✓
```

**Additional checks**:
- Wi-Fi RSSI matches (dBm value)
- IP address is correct
- Device ID and name are correct
- Free memory is reasonable (>100KB)

**✅ Data accuracy verified**

---

### Test 4.3: Test API Key Change (Security)

**Objective**: Verify changing API key requires code update

**Steps**:
1. On Raspberry Pi, edit `config.py`:
   ```bash
   nano config.py
   ```

2. Change the API key:
   ```python
   API_KEY = os.getenv('API_KEY', 'new-secure-key-12345')
   ```

3. Restart Flask server:
   - Press Ctrl+C in Flask terminal
   - Run `python3 app.py` again

4. Watch Serial Monitor on ESP32
   
   Expected output:
   ```
   [HTTP] Response Code: 401
   [ERROR] Unauthorized - Invalid API key!
   ```

5. Update ESP32 code with new key:
   ```cpp
   const char* api_key = "new-secure-key-12345";
   ```

6. Upload updated code to ESP32

7. Monitor Serial Monitor
   
   Expected output:
   ```
   [HTTP] Response Code: 200
   [SUCCESS] Data sent successfully!
   ```

**✅ API key security working correctly**

---

## 🎨 Phase 5: Dashboard UI Testing

### Test 5.1: Access Dashboard

**Objective**: Verify dashboard loads in browser

**Steps**:
1. Open web browser on your laptop
2. Navigate to:
   ```
   http://<raspberry-pi-ip>:5000
   ```
   (Example: `http://192.168.1.10:5000`)

**Expected Result**:
- Dashboard page loads successfully
- No blank or error screens
- Page displays within 2 seconds

**If Failed**:
- Check Raspberry Pi IP is correct
- Verify Flask server is running
- Check browser console for errors (F12)

**✅ Dashboard loads successfully**

---

### Test 5.2: Verify Dashboard Data Display

**Objective**: Ensure all device information displays correctly

**Steps**:
1. Once dashboard loads, verify these sections appear:
   - [ ] Header with project title
   - [ ] Status card showing "ONLINE" or "OFFLINE"
   - [ ] Green pulsing indicator (if online)
   - [ ] Device ID card
   - [ ] Device Name card
   - [ ] IP Address card
   - [ ] Wi-Fi Signal card
   - [ ] Uptime card
   - [ ] Free Memory card
   - [ ] Firmware Version card
   - [ ] Last Seen card
   - [ ] Security section
   - [ ] Footer with links

2. Verify data matches your ESP32:
   ```
   Expected: ESP32-Node-01  →  Actual: [check dashboard]
   Expected: 192.168.1.25   →  Actual: [check dashboard]
   Expected: -52 dBm        →  Actual: [check dashboard]
   ```

**Expected Result**:
```
Device Status: 🟢 ONLINE

Device ID: ESP32-Node-01
Device Name: Living Room ESP32
IP Address: 192.168.1.25
Wi-Fi Signal: -52 dBm (Excellent)
Uptime: 2m 45s
Free Memory: 183.04 KB
Firmware: 1.0.0
Last Seen: 3s ago

Security Status:
✓ API Key Protection: Enabled
✓ Authentication: X-API-Key Header
Last Valid Update: Just now
Protocol: HTTP (Local Network)
```

**✅ Dashboard displays all data correctly**

---

### Test 5.3: Verify Auto-Refresh

**Objective**: Ensure dashboard updates every 5 seconds

**Steps**:
1. Keep dashboard open in browser
2. Watch the "Uptime" value
3. Wait 10 seconds
4. Check if uptime increased by ~10 seconds

**Expected Behavior**:
```
Time 0s:  Uptime: 2m 30s
Time 5s:  Uptime: 2m 35s  (updated automatically)
Time 10s: Uptime: 2m 40s  (updated automatically)
Time 15s: Uptime: 2m 45s  (updated automatically)
```

The "Last Seen" should show "Just now" or "0s ago"

**If Not Updating**:
- Open browser console (F12)
- Check for JavaScript errors
- Refresh page manually
- Check Flask server is running

**✅ Auto-refresh working**

---

### Test 5.4: Test Responsive Design

**Objective**: Verify dashboard works on different screen sizes

**Steps**:
1. On desktop browser, press F12 to open Developer Tools
2. Click device toolbar icon (mobile view)
3. Test on different sizes:
   - [ ] Mobile (320px)
   - [ ] Tablet (768px)
   - [ ] Desktop (1200px)

4. On each screen size, verify:
   - Text is readable
   - Cards stack properly
   - No horizontal scrolling
   - All elements visible

**Expected Result**:
- Mobile: Cards stack vertically
- Tablet: 2-column grid
- Desktop: Multi-column grid

**✅ Responsive design verified**

---

### Test 5.5: Test Offline Detection UI

**Objective**: Verify dashboard shows offline state correctly

**Steps**:
1. Dashboard is showing device as "ONLINE" with green indicator
2. Turn OFF the ESP32 board
3. Wait 25 seconds (device has 20-second timeout + 5 seconds refresh)
4. Refresh dashboard or wait for auto-refresh
5. Check status indicator

**Expected Result**:
```
Device Status: 🔴 OFFLINE
Status indicator turns RED
Status card background changes to red
Uptime stops changing
Last Seen: 25s ago
```

**If Status Still Shows Online**:
- Wait another 5 seconds for dashboard refresh
- Manually refresh page
- Check Flask server received the status change

**Retest Online**:
1. Turn ESP32 back on
2. Wait 10 seconds
3. Check dashboard

**Expected Result**:
```
Device Status: 🟢 ONLINE
Status indicator turns GREEN
Data updates resume
```

**✅ Offline detection in UI working**

---

### Test 5.6: Test Wi-Fi Signal Quality Display

**Objective**: Verify Wi-Fi signal description is accurate

**Steps**:
1. Check dashboard shows Wi-Fi signal (dBm value)
2. Compare with expected quality:

   | RSSI (dBm) | Quality | Expected Label |
   |----------|---------|----------------|
   | -30 to -50 | Excellent | 🟢 Excellent |
   | -51 to -60 | Very Good | 🟢 Very Good |
   | -61 to -70 | Good | 🟢 Good |
   | -71 to -80 | Fair | 🟡 Fair |
   | -81+ | Weak | 🔴 Weak |

3. Check ESP32 Serial Monitor for actual RSSI
4. Verify dashboard shows matching quality level

**Example**:
```
ESP32 shows: Signal Strength: -55 dBm
Dashboard shows: Wi-Fi Signal: -55 dBm (🟢 Very Good)
→ Correct mapping ✓
```

**✅ Wi-Fi signal display accurate**

---

### Test 5.7: Test Uptime Format

**Objective**: Verify uptime displays in readable format

**Steps**:
1. Note uptime_seconds from API response
2. Check dashboard displays correctly

**Examples**:
```
API shows: "uptime_seconds": 5
Dashboard shows: "5s"

API shows: "uptime_seconds": 45
Dashboard shows: "45s"

API shows: "uptime_seconds": 120
Dashboard shows: "2m 0s"

API shows: "uptime_seconds": 3665
Dashboard shows: "1h 1m"
```

**✅ Uptime format verified**

---

## 🔒 Phase 6: Security Testing

### Test 6.1: Verify API Key Authentication

**Objective**: Confirm API key validation prevents unauthorized access

**Steps**:
1. Try to send data WITHOUT API key:
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -d '{"device_id":"test"}'
   ```

2. Try with WRONG API key:
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -H "X-API-Key: wrong-key" \
     -d '{"device_id":"test"}'
   ```

3. Try with CORRECT API key:
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -H "X-API-Key: change-this-secret-key" \
     -d '{"device_id":"test"}'
   ```

**Expected Results**:
```
No API key: 401 Unauthorized {"status":"error","message":"Invalid API key"}
Wrong key:  401 Unauthorized {"status":"error","message":"Invalid API key"}
Correct key: 200 OK {"status":"success",...}
```

**✅ API key authentication working**

---

### Test 6.2: Test Invalid JSON Handling

**Objective**: Verify server handles malformed data gracefully

**Steps**:
1. Send malformed JSON:
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -H "X-API-Key: change-this-secret-key" \
     -d '{invalid json}'
   ```

**Expected Result**:
Server should not crash. Response should indicate error (or silently reject).

**✅ Error handling verified**

---

### Test 6.3: Test Missing Required Fields

**Objective**: Verify server handles incomplete data

**Steps**:
1. Send data with missing fields:
   ```bash
   curl -X POST http://<raspberry-pi-ip>:5000/api/device/update \
     -H "Content-Type: application/json" \
     -H "X-API-Key: change-this-secret-key" \
     -d '{"device_id": "test"}'  # Missing other fields
   ```

**Expected Result**:
- Server accepts the partial data
- Server should still work with what it has
- Dashboard should display "--" for missing fields

**✅ Graceful handling of incomplete data**

---

## 📈 Phase 7: Long-Term Reliability Testing

### Test 7.1: 1-Hour Continuous Operation

**Objective**: Verify system runs stable for 1 hour

**Setup**:
- [ ] Flask server running on Raspberry Pi
- [ ] ESP32 powered and transmitting
- [ ] Dashboard open in browser

**Duration**: 60 minutes

**What to Monitor**:
1. **Flask Server Terminal**:
   - No error messages
   - Regular "Device data updated" logs
   - No memory leaks (free memory doesn't decrease)

2. **ESP32 Serial Monitor**:
   - Successful sends every 10 seconds
   - No failed HTTP requests
   - Uptime increases normally

3. **Dashboard**:
   - Auto-refreshes every 5 seconds
   - Data updates correctly
   - No JavaScript errors (F12 console)
   - Status remains "ONLINE"

**Expected Result After 1 Hour**:
```
ESP32 Uptime: ~3600 seconds (1 hour)
Last Seen: 0-5 seconds ago
Status: ONLINE
No crashes or errors
```

**✅ 1-hour stability verified**

---

### Test 7.2: Device Disconnect/Reconnect

**Objective**: Verify system handles device going offline

**Setup**:
- Dashboard is open and showing device as ONLINE
- System has been running for at least 5 minutes

**Steps**:
1. **T=0s**: Device is showing as ONLINE
   - Record last_seen time
   - Record uptime

2. **T=5s**: Turn OFF ESP32 board
   - Do NOT send new data

3. **T=20s**: Check ESP32 Serial Monitor
   - Should show Wi-Fi disconnected or trying to reconnect

4. **T=25s**: Check dashboard
   - Should show "OFFLINE"
   - Status indicator should be RED

5. **T=30s**: Turn ESP32 back ON
   - Watch Serial Monitor for "Wi-Fi connected!"

6. **T=40s**: Check dashboard
   - Should return to "ONLINE"
   - Uptime should reset to ~10 seconds
   - Status should be GREEN
   - Last Seen should be recent

**Expected Timeline**:
```
T=0s:   ONLINE, Last Seen: 0s ago, Uptime: 300s
T=25s:  OFFLINE, Last Seen: 25s ago, Uptime: still 300s
T=40s:  ONLINE, Last Seen: 0s ago, Uptime: ~10s
```

**✅ Device reconnection handled properly**

---

### Test 7.3: Network Reconnection

**Objective**: Verify system handles Wi-Fi dropout

**Setup**:
- System running normally
- Both devices connected

**Steps**:
1. Temporarily disable Wi-Fi on your router (5-10 seconds)
2. Watch Serial Monitor on ESP32
   - Should show Wi-Fi disconnection
   - Should attempt to reconnect

3. Re-enable Wi-Fi
4. Wait 10 seconds
5. Check dashboard
   - Should return to ONLINE

**✅ Network recovery verified**

---

### Test 7.4: Memory Stability (24-Hour Test)

**Objective**: Ensure no memory leaks over extended period

**Setup**:
- System running for 24 hours minimum
- Monitor free memory periodically

**What to Monitor**:
1. **Check every 4 hours**:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/device/status | grep free_heap
   ```

2. **Record results**:
   ```
   Hour 0:  free_heap: 187432
   Hour 4:  free_heap: 187200  (OK, small variance)
   Hour 8:  free_heap: 187000  (OK)
   Hour 12: free_heap: 186800  (OK)
   Hour 16: free_heap: 186500  (OK)
   Hour 20: free_heap: 186200  (OK)
   Hour 24: free_heap: 186000  (OK)
   ```

3. **Expected Behavior**:
   - Memory stays relatively stable
   - No sudden drops
   - No continuous decrease

**If Memory Drops Continuously**:
- Indicates a memory leak
- May need to optimize code
- Consider restarting devices periodically

**✅ Memory stability verified (or leak identified)**

---

## 🆘 Troubleshooting

### Issue: "Cannot connect to Raspberry Pi"

**Symptoms**:
```
ping: cannot resolve host 192.168.1.10
curl: (7) Failed to connect to 192.168.1.10 port 5000
```

**Solutions**:
1. **Verify IP address**:
   ```bash
   # On Raspberry Pi
   hostname -I
   ```

2. **Check both on same network**:
   ```bash
   # On laptop
   ipconfig (Windows) or ifconfig (Mac/Linux)
   # Look for 192.168.x.x address - should match Raspberry Pi network
   ```

3. **Test basic connectivity**:
   ```bash
   ping 192.168.1.10
   ```

4. **Check Raspberry Pi is powered**:
   - Red LED should be on
   - Green LED should blink occasionally

5. **Restart Raspberry Pi**:
   ```bash
   sudo reboot
   ```

---

### Issue: "Port 5000 is already in use"

**Symptoms**:
```
OSError: [Errno 48] Address already in use
```

**Solution**:
```bash
# Find process using port 5000
sudo lsof -i :5000

# Kill the process
sudo kill -9 <PID>

# Or use different port
# Edit app.py: app.run(port=5001)
```

---

### Issue: "ESP32 Won't Connect to Wi-Fi"

**Symptoms**:
```
[INFO] Connecting to Wi-Fi...
...........
[ERROR] Failed to connect to Wi-Fi
```

**Solutions**:
1. **Check SSID and password**:
   - No extra spaces
   - Correct capitalization
   - No special characters (use simple SSID first)

2. **Verify 2.4 GHz Wi-Fi**:
   - ESP8266 doesn't support 5 GHz
   - ESP32 supports 5 GHz but 2.4 GHz is more reliable

3. **Move ESP32 closer to router**

4. **Restart router and board**

5. **Try WPS connection**:
   ```cpp
   // Try WPS instead of hardcoded credentials
   WiFi.beginSmartConfig();
   ```

---

### Issue: "API Returns 401 Unauthorized"

**Symptoms**:
```json
{"status": "error", "message": "Invalid API key"}
```

**Solutions**:
1. **Check API key matches**:
   - ESP32 code: `const char* api_key = "..."`
   - Flask config: `API_KEY = os.getenv(..., '...')`
   - Must be identical

2. **Check header name**:
   - Should be exactly: `X-API-Key`
   - Not `API-Key` or `Authorization`

3. **Look for typos**:
   ```bash
   # Compare strings byte by byte
   echo -n "change-this-secret-key" | wc -c
   ```

4. **Verify header is sent**:
   ```bash
   curl -v http://... -H "X-API-Key: change-this-secret-key"
   ```
   Look for header in output

---

### Issue: "Dashboard Shows No Data"

**Symptoms**:
- Dashboard loads but all cards show "--"
- Status shows "OFFLINE"

**Solutions**:
1. **Verify ESP32 has sent data**:
   - Check Serial Monitor shows success messages
   - Check Flask server has received data (logs)

2. **Verify API endpoint works**:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/device/status
   ```

3. **Check browser console**:
   - Press F12
   - Check Console tab for JavaScript errors
   - Check Network tab to see if `/api/device/status` is working

4. **Verify Flask server is running**:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/health
   ```

---

### Issue: "Dashboard Doesn't Auto-Refresh"

**Symptoms**:
- Data doesn't update every 5 seconds
- Uptime stays frozen
- Manual refresh works

**Solutions**:
1. **Check browser console**:
   - F12 → Console tab
   - Look for JavaScript errors

2. **Check network requests**:
   - F12 → Network tab
   - Reload page
   - Look for `/api/device/status` requests
   - Check response status (should be 200)

3. **Verify Flask is running**:
   ```bash
   curl http://<raspberry-pi-ip>:5000/api/device/status
   ```

4. **Try different browser**:
   - Clear cache (Ctrl+Shift+Del)
   - Try Chrome, Firefox, Safari
   - Try private/incognito window

---

### Issue: "ESP32 Shows 'Connection refused'"

**Symptoms**:
```
[ERROR] HTTP request failed: Connection refused
```

**Solutions**:
1. **Verify Raspberry Pi is running**:
   ```bash
   ping <raspberry-pi-ip>
   ```

2. **Verify Flask server is running**:
   - Check Raspberry Pi terminal shows "Running on http://0.0.0.0:5000"
   - Restart if not running

3. **Check firewall**:
   ```bash
   # On Raspberry Pi
   sudo ufw status
   sudo ufw allow 5000/tcp
   ```

4. **Verify IP address is correct**:
   - Update ESP32 code with correct Raspberry Pi IP
   - Re-upload

---

## ✅ Final Verification Checklist

After completing all tests, verify:

- [ ] **Phase 1**: Network connectivity established
- [ ] **Phase 2**: Flask server running and responding
- [ ] **Phase 3**: ESP32 connected and sending data
- [ ] **Phase 4**: Continuous data flow verified
- [ ] **Phase 5**: Dashboard displays correctly
- [ ] **Phase 6**: Security features working
- [ ] **Phase 7**: Long-term reliability confirmed

### Project Status Summary

```
✅ Hardware: Connected
✅ Server: Running
✅ Device: Transmitting
✅ API: Responding
✅ Dashboard: Displaying
✅ Security: Validated
✅ Stability: Confirmed

Status: PRODUCTION READY 🚀
```

---

## 🎯 Next Steps After Testing

1. **Document Results**
   - Screenshot successful dashboard
   - Record successful API responses
   - Keep this guide for reference

2. **Push to Portfolio**
   - Add test results to GitHub README
   - Create screenshots section
   - Add test evidence

3. **Consider Improvements**
   - Database logging
   - Multiple device support
   - Email alerts
   - Data visualization

4. **Share with Others**
   - Link to GitHub repo
   - Explain testing methodology
   - Show working dashboard

---

**Testing Complete! Your IoT Dashboard is ready for production. 🎉**
