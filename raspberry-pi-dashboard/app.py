#!/usr/bin/env python3
"""
IoT Device Health & Security Dashboard - Flask Backend

This Flask application serves a web dashboard that displays real-time
device health information from an ESP32/NodeMCU IoT device.
"""

from flask import Flask, render_template, jsonify, request
from datetime import datetime, timedelta
import logging
import os
from config import (
    API_KEY,
    OFFLINE_TIMEOUT_SECONDS,
    FLASK_HOST,
    FLASK_PORT,
    FLASK_DEBUG
)

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# Initialize Flask app
app = Flask(__name__)
app.config['JSON_SORT_KEYS'] = False

# Global device data storage (in-memory)
device_data = {}
last_update_time = None


@app.route('/')
def dashboard():
    """
    Serve the main dashboard HTML page
    """
    logger.info('Dashboard page requested')
    return render_template('index.html')


@app.route('/api/device/update', methods=['POST'])
def update_device_data():
    """
    Receive device health data from ESP32/NodeMCU
    
    Expected JSON payload:
    {
        "device_id": "ESP32-Node-01",
        "device_name": "Living Room ESP32",
        "ip_address": "192.168.1.25",
        "wifi_rssi": -52,
        "uptime_seconds": 3560,
        "free_heap": 187432,
        "firmware_version": "1.0.0"
    }
    """
    try:
        # Validate API key
        api_key = request.headers.get('X-API-Key')
        if api_key != API_KEY:
            logger.warning(f'Invalid API key attempt from {request.remote_addr}')
            return jsonify({
                'status': 'error',
                'message': 'Invalid API key'
            }), 401

        # Parse JSON data
        data = request.get_json()
        if not data:
            return jsonify({
                'status': 'error',
                'message': 'No JSON data provided'
            }), 400

        # Store device data globally
        global device_data, last_update_time
        device_data = data
        last_update_time = datetime.now()

        logger.info(f'Device data updated from {data.get("device_id", "unknown")}')
        
        return jsonify({
            'status': 'success',
            'message': 'Device data updated',
            'timestamp': last_update_time.isoformat()
        }), 200

    except Exception as e:
        logger.error(f'Error updating device data: {str(e)}')
        return jsonify({
            'status': 'error',
            'message': 'Internal server error'
        }), 500


@app.route('/api/device/status', methods=['GET'])
def get_device_status():
    """
    Get the latest device status
    
    Returns device health data with online/offline status
    """
    try:
        # Check if we have any device data
        if not device_data:
            return jsonify({
                'status': 'error',
                'message': 'No device data available'
            }), 404

        # Calculate device status based on last update time
        if last_update_time is None:
            device_status = 'offline'
        else:
            time_since_update = (datetime.now() - last_update_time).total_seconds()
            device_status = 'online' if time_since_update <= OFFLINE_TIMEOUT_SECONDS else 'offline'

        # Prepare response
        response = device_data.copy()
        response['status'] = device_status
        response['last_seen'] = last_update_time.isoformat() if last_update_time else None
        response['time_since_update'] = int((datetime.now() - last_update_time).total_seconds()) if last_update_time else None

        return jsonify(response), 200

    except Exception as e:
        logger.error(f'Error getting device status: {str(e)}')
        return jsonify({
            'status': 'error',
            'message': 'Internal server error'
        }), 500


@app.route('/api/health', methods=['GET'])
def health_check():
    """
    Health check endpoint
    """
    return jsonify({
        'status': 'ok',
        'timestamp': datetime.now().isoformat(),
        'api_version': '1.0.0'
    }), 200


@app.errorhandler(404)
def not_found(error):
    """
    Handle 404 errors
    """
    return jsonify({
        'status': 'error',
        'message': 'Endpoint not found'
    }), 404


@app.errorhandler(500)
def internal_error(error):
    """
    Handle 500 errors
    """
    logger.error(f'Internal server error: {str(error)}')
    return jsonify({
        'status': 'error',
        'message': 'Internal server error'
    }), 500


if __name__ == '__main__':
    logger.info('Starting IoT Device Health Dashboard')
    logger.info(f'API Key: {"*" * (len(API_KEY) - 4) + API_KEY[-4:]}')
    logger.info(f'Offline timeout: {OFFLINE_TIMEOUT_SECONDS} seconds')
    
    app.run(
        host=FLASK_HOST,
        port=FLASK_PORT,
        debug=FLASK_DEBUG,
        use_reloader=False
    )
