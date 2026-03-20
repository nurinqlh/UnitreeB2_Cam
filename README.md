

---

## System

```
Unitree B2 Robot
│
├── Camera Hardware (192.168.123.161)
│   ├── Front Camera → rtsp://192.168.123.161:8551/front_video
│   └── Back Camera  → rtsp://192.168.123.161:8552/back_video
│
├── MediaMTX (mediamtx.yml)
│   ├── Reads RTSP streams from cameras
│   ├── Transcodes to 640x360 (low latency)
│   └── Serves as WebRTC on port 8889
│
├── ROS2 Nodes
│   ├── front_cam.cpp  → publishes /front_camera/image_raw
│   ├── back_cam.cpp   → publishes /back_camera/image_raw
│   └── rtsp_cam_bridge.py → Python alternative to C++ nodes
│
├── Rosbridge (port 9090)
│   └── Bridges ROS2 topics to WebSocket for dashboard
│
└── Web Dashboard (port 8000)
    ├── dashboard.html → main dashboard page
    ├── app.js         → ROS2 data handling
    └── navigation.js  → map and navigation control
```

---

## Installation

### 1. Install MediaMTX
```bash
wget https://github.com/bluenviron/mediamtx/releases/download/v1.9.1/mediamtx_v1.9.1_linux_amd64.tar.gz
tar -xzf mediamtx_v1.9.1_linux_amd64.tar.gz
```

### 2. Install Python dependencies
```bash
pip install opencv-python --break-system-packages
```

---

## Usage

**Terminal 1 — Web server:**
```bash
cd robot_web_interface
python3 -m http.server 8000
```

**Terminal 2 — Rosbridge:**
```bash
ros2 launch rosbridge_server rosbridge_websocket_launch.xml
```

**Terminal 3 — MediaMTX:**
```bash
./mediamtx
```

**Terminal 4 — Camera bridge:**
```bash
python3 rtsp_cam_bridge.py
```

### Access dashboard
```
http://<robot-ip>:8000/dashboard.html
```

## Video Streaming

Video is streamed using **WebRTC** via MediaMTX:

| Stream | Resolution | URL |
|---|---|---|
| Front Camera (HD) | 1280x720 | `http://<ip>:8889/front_cam` |
| Front Camera (Low) | 640x360 | `http://<ip>:8889/front_cam_low` |
| Back Camera (HD) | 1280x720 | `http://<ip>:8889/back_cam` |
| Back Camera (Low) | 640x360 | `http://<ip>:8889/back_cam_low` |

---

## Latency Comparison

| Method | Delay |
|---|---|
| Original (broken) | No video |
| RTSP → ROS → Rosbridge → Browser | 2-3 seconds |
| RTSP → MediaMTX → WebRTC → Browser | ~200-500ms |

---
