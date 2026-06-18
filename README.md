# IoT Device Health & Security Dashboard

A beginner-friendly IoT monitoring system built with **ESP32/NodeMCU** and **Raspberry Pi Zero W**. Monitor device health information including Wi-Fi signal strength, uptime, memory usage, and device availability in real-time.

**No external sensors required** — the system uses the IoT device's built-in telemetry.

---

## ✨ Features

- 📡 **Real-time Device Monitoring**: Track Wi-Fi strength, uptime, memory, and IP address
- 🟢 **Online/Offline Detection**: Automatic status based on heartbeat timeout
- 🔐 **API Key Authentication**: Basic security using HTTP header validation
- 📊 **Live Web Dashboard**: Responsive HTML/CSS/JavaScript interface
- 🔄 **Auto-Refresh**: Dashboard updates every 5 seconds
- ⚙️ **No Sensors Needed**: Uses device's internal metrics
- 📱 **Portfolio-Ready**: Clean, professional code structure

---

## 🛠️ Hardware Required

| Component | Quantity | Purpose |
|-----------|----------|----------|
| ESP32 or NodeMCU ESP8266 | 1 | IoT Device |
| Raspberry Pi Zero W | 1 | Dashboard Server |
| microSD Card (8GB+) | 1 | Raspberry Pi OS |
| USB Power Cable | 2 | Power supply |
| Wi-Fi Network | Required | Device communication |

**Note**: No external sensors needed.

---

## 💻 Technologies Used

### ESP32/NodeMCU
- Arduino IDE
- C++ with ESP32 libraries
- HTTP Client for API requests
- Built-in Wi-Fi & system metrics

### Raspberry Pi Server
- **Framework**: Python Flask
- **Backend**: RESTful API
- **Frontend**: HTML5, CSS3, JavaScript

### Protocols & APIs
- HTTP POST/GET
- JSON data format
- API Key authentication (X-API-Key header)

---

## 📐 System Architecture

```
┌─────────────────────────┐
│   ESP32 / NodeMCU       │
│  (IoT Device)           │
│ - Wi-Fi RSSI            │
│ - Uptime                │
│ - Free Memory           │
│ - IP Address            │
└────────────┬────────────┘
             │
             │ HTTP POST (every 10s)
             │ JSON + API Key
             │
             ▼
┌─────────────────────────┐
│ Raspberry Pi Zero W     │
│ (Flask Server)          │
│ - Validate API Key      │
│ - Store Latest Data     │
│ - Calculate Status      │
│ - Serve Dashboard       │
└────────────┬────────────┘
             │
             │ GET /api/device/status
             │ Live JSON Updates
             │
             ▼
┌─────────────────────────┐
│  Web Browser            │
│  (Dashboard UI)         │
│ - Live Status Display   │
│ - Device Information    │
│ - Real-time Updates     │
└─────────────────────────┘
```

---

## 📁 Folder Structure

```
iot-device-health-dashboard/
│
├── esp32-device/
│   ├── esp32_device_monitor.ino    # ESP32/NodeMCU firmware
│   └── README.md                   # Device setup guide
│
├── raspberry-pi-dashboard/
│   ├── app.py                      # Flask backend
│   ├── requirements.txt            # Python dependencies
│   ├── config.py                   # Configuration (API key, etc)
│   ├── templates/
│   │   └── index.html              # Dashboard HTML
│   └── static/
│       ├── style.css               # Dashboard styling
│       └── script.js               # Dashboard JavaScript
│
├── docs/
│   ├── API_SPECIFICATION.md        # Complete API documentation
│   ├── SETUP_GUIDE.md              # Step-by-step setup
│   └── TROUBLESHOOTING.md          # Common issues & fixes
│
├── images/
│   ├── system-architecture.png     # Architecture diagram
│   ├── dashboard-screenshot.png    # UI screenshot
│   └── hardware-setup.jpg          # Physical setup
│
├── LICENSE                         # MIT License
└── README.md                       # This file
```

---

## 🚀 Quick Start

### Prerequisites
- Git installed
- GitHub account
- ESP32 or NodeMCU board
- Raspberry Pi Zero W with Raspberry Pi OS

### 1. Clone Repository
```bash
git clone https://github.com/SeninduP/iot-device-health-dashboard.git
cd iot-device-health-dashboard
```

### 2. ESP32/NodeMCU Setup
See `esp32-device/README.md` for detailed instructions:
- Install Arduino IDE
- Configure Wi-Fi credentials
- Set Raspberry Pi IP address
- Upload firmware
- Verify data in Serial Monitor

### 3. Raspberry Pi Setup
See `docs/SETUP_GUIDE.md` for detailed instructions:
```bash
# Install dependencies
pip install -r raspberry-pi-dashboard/requirements.txt

# Run Flask server
python raspberry-pi-dashboard/app.py
```

### 4. Access Dashboard
Open browser and navigate to:
```
http://<raspberry-pi-ip>:5000
```

---

## 📡 API Specification

### Endpoint 1: Send Device Data
**POST** `/api/device/update`

**Headers:**
```http
Content-Type: application/json
X-API-Key: change-this-secret-key
```

**Request Body:**
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

**Success Response (200):**
```json
{
  "status": "success",
  "message": "Device data updated"
}
```

**Unauthorized Response (401):**
```json
{
  "status": "error",
  "message": "Invalid API key"
}
```

---

### Endpoint 2: Get Device Status
**GET** `/api/device/status`

**Response:**
```json
{
  "device_id": "ESP32-Node-01",
  "device_name": "Living Room ESP32",
  "ip_address": "192.168.1.25",
  "wifi_rssi": -52,
  "uptime_seconds": 3560,
  "free_heap": 187432,
  "firmware_version": "1.0.0",
  "last_seen": "2026-06-18 15:30:20",
  "status": "online"
}
```

---

## 🔒 Security Concept

This project demonstrates **basic API key authentication** — a fundamental IoT security concept.

### How It Works
1. ESP32 includes `X-API-Key` header in every request
2. Raspberry Pi validates the key before accepting data
3. Invalid requests are rejected with 401 Unauthorized

### Current Limitations (Beginner Project)
- ⚠️ No HTTPS encryption (HTTP only)
- ⚠️ API key stored in device firmware
- ⚠️ No dashboard login
- ⚠️ No rate limiting

### Production Improvements
- Use HTTPS/TLS encryption
- Store API key in environment variables
- Add request rate limiting
- Implement dashboard authentication
- Add IP whitelisting
- Log failed authentication attempts
- Use device-specific API keys

---

## 📊 Data Fields Explanation

| Field | Type | Example | Description |
|-------|------|---------|-------------|
| `device_id` | String | "ESP32-Node-01" | Unique device identifier |
| `device_name` | String | "Living Room ESP32" | Human-readable device name |
| `ip_address` | String | "192.168.1.25" | Local network IP address |
| `wifi_rssi` | Integer | -52 | Wi-Fi signal strength (dBm) |
| `uptime_seconds` | Integer | 3560 | Time since device startup |
| `free_heap` | Integer | 187432 | Available RAM in bytes |
| `firmware_version` | String | "1.0.0" | Device firmware version |
| `last_seen` | String | "2026-06-18 15:30:20" | Server-side timestamp (added by Raspberry Pi) |
| `status` | String | "online" / "offline" | Calculated by server |

---

## 🎯 Online/Offline Detection Logic

The server detects device status based on heartbeat timeout:

```
If (current_time - last_seen) ≤ 20 seconds:
    Device Status = "online"
Else:
    Device Status = "offline"
```

Since the ESP32 sends data every 10 seconds, a 20–30 second threshold is appropriate.

---

## 📈 Future Improvements

- [ ] SQLite database for historical data logging
- [ ] Support for multiple IoT devices
- [ ] Dashboard login page
- [ ] Dark mode UI
- [ ] Email/Telegram alerts for offline devices
- [ ] Historical charts (Wi-Fi strength over time)
- [ ] HTTPS support with reverse proxy
- [ ] Docker containerization
- [ ] Device activity logs
- [ ] CSV data export

---

## 👨‍💻 Author

**Senindu Pathirana**  
Computer Science & Cybersecurity Undergraduate  
GitHub: [@SeninduP](https://github.com/SeninduP)

---

## 📜 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) file for details.

---

## 📞 Support

For issues, questions, or improvements:
1. Check documentation in `docs/` folder
2. Review setup guides and troubleshooting
3. Open a GitHub Issue

---

**Last Updated**: June 2026  
**Status**: Active Development