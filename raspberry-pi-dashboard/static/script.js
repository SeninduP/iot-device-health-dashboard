// Configuration
const UPDATE_INTERVAL = 5000; // Update dashboard every 5 seconds
const API_ENDPOINT = '/api/device/status';

// Global state
let deviceData = null;
let lastUpdateTime = null;

// Initialize
document.addEventListener('DOMContentLoaded', function() {
    console.log('Dashboard initialized');
    fetchDeviceStatus();
    setInterval(fetchDeviceStatus, UPDATE_INTERVAL);
});

/**
 * Fetch device status from API
 */
function fetchDeviceStatus() {
    fetch(API_ENDPOINT)
        .then(response => {
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            return response.json();
        })
        .then(data => {
            console.log('Device status received:', data);
            deviceData = data;
            lastUpdateTime = new Date();
            updateDashboard(data);
        })
        .catch(error => {
            console.error('Error fetching device status:', error);
            showError();
        });
}

/**
 * Update dashboard with device data
 */
function updateDashboard(data) {
    // Update timestamp
    document.getElementById('timestamp').textContent = new Date().toLocaleTimeString();

    // Update status
    const statusCard = document.getElementById('status-card');
    const statusValue = document.getElementById('status-value');
    const isOnline = data.status === 'online';

    statusCard.classList.remove('online', 'offline');
    statusCard.classList.add(isOnline ? 'online' : 'offline');
    statusValue.textContent = isOnline ? '🟢 ONLINE' : '🔴 OFFLINE';

    // Update device information
    document.getElementById('device-id').textContent = data.device_id || '--';
    document.getElementById('device-name').textContent = data.device_name || '--';
    document.getElementById('ip-address').textContent = data.ip_address || '--';

    // Update Wi-Fi signal
    const rssi = data.wifi_rssi || 0;
    document.getElementById('wifi-rssi').textContent = rssi + ' dBm';
    document.getElementById('wifi-label').textContent = getSignalQuality(rssi);

    // Update uptime
    document.getElementById('uptime').textContent = formatUptime(data.uptime_seconds || 0);

    // Update free memory
    const freeMemoryMB = ((data.free_heap || 0) / 1024).toFixed(2);
    document.getElementById('free-memory').textContent = freeMemoryMB + ' KB';

    // Update firmware version
    document.getElementById('firmware-version').textContent = data.firmware_version || '--';

    // Update last seen
    const lastSeenSeconds = getSecondsSince(data.last_seen);
    document.getElementById('last-seen').textContent = lastSeenSeconds + 's ago';
    document.getElementById('last-valid-update').textContent = getReadableTime(data.last_seen);
}

/**
 * Get Wi-Fi signal quality description
 */
function getSignalQuality(rssi) {
    if (rssi >= -50) return '🟢 Excellent';
    if (rssi >= -60) return '🟢 Very Good';
    if (rssi >= -70) return '🟡 Good';
    if (rssi >= -80) return '🟡 Fair';
    return '🔴 Weak';
}

/**
 * Format uptime into readable format
 */
function formatUptime(seconds) {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;

    if (hours > 0) {
        return `${hours}h ${minutes}m`;
    } else if (minutes > 0) {
        return `${minutes}m ${secs}s`;
    } else {
        return `${secs}s`;
    }
}

/**
 * Get seconds since timestamp
 */
function getSecondsSince(timestamp) {
    if (!timestamp) return '--';
    
    const lastSeenTime = new Date(timestamp);
    const currentTime = new Date();
    const secondsDiff = Math.floor((currentTime - lastSeenTime) / 1000);
    
    return Math.max(0, secondsDiff);
}

/**
 * Get readable time format
 */
function getReadableTime(timestamp) {
    if (!timestamp) return 'Unknown';
    
    const date = new Date(timestamp);
    const now = new Date();
    const secondsDiff = (now - date) / 1000;
    
    if (secondsDiff < 10) return 'Just now';
    if (secondsDiff < 60) return Math.floor(secondsDiff) + 's ago';
    if (secondsDiff < 3600) return Math.floor(secondsDiff / 60) + 'm ago';
    if (secondsDiff < 86400) return Math.floor(secondsDiff / 3600) + 'h ago';
    
    return date.toLocaleTimeString();
}

/**
 * Show error state
 */
function showError() {
    document.getElementById('status-card').classList.add('offline');
    document.getElementById('status-value').textContent = '❌ No Connection';
    document.getElementById('status-card').classList.remove('online');
}

// Console logging for debugging
console.log('Dashboard script loaded');
console.log('Update interval:', UPDATE_INTERVAL + 'ms');
console.log('API endpoint:', API_ENDPOINT);
