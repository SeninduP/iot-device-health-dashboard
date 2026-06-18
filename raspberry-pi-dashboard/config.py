# Flask Configuration

import os
from datetime import datetime

# API Configuration
API_KEY = os.getenv('API_KEY', 'change-this-secret-key')
OFFLINE_TIMEOUT_SECONDS = 20  # Device considered offline after 20 seconds

# Server Configuration
FLASK_HOST = '0.0.0.0'  # Accessible from all network interfaces
FLASK_PORT = 5000
FLASK_DEBUG = False  # Set to True only for development

# CORS Configuration (for future multi-device support)
CORS_ORIGINS = ['*']  # Allow all origins for now

# Logging Configuration
LOG_LEVEL = 'INFO'
LOG_FORMAT = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'

# Device Configuration
DEFAULT_DEVICE_ID = 'ESP32-Node-01'
DEFAULT_DEVICE_NAME = 'Living Room ESP32'

# Response Configuration
SUCCESS_RESPONSE = {
    'status': 'success',
    'message': 'Device data updated',
    'timestamp': datetime.now().isoformat()
}

ERROR_RESPONSE = {
    'status': 'error',
    'message': 'Invalid API key',
    'timestamp': datetime.now().isoformat()
}
