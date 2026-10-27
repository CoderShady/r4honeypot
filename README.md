# 🛡️ r4honeypot (Arduino Uno R4 WiFi)

[![Arduino Board](https://img.shields.io/badge/Board-Arduino%20Uno%20R4%20WiFi-blue?logo=arduino)](https://store.arduino.cc/products/uno-r4-wifi)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Security: Defensive](https://img.shields.io/badge/Security-Blue%20Team%2--Defensive-success)]()

A lightweight, plug-and-play **hardware-based network tripwire and decoy honeypot** built entirely on the bare **Arduino Uno R4 WiFi**. Designed for local subnets, this node emulates high-value enterprise services to instantly log unauthorized reconnaissance, port scans, and lateral movement with near-zero false positives.

Features a real-time **generative dual-sine wave visualization engine** running natively on the board's 12×8 LED matrix that dynamically shifts from calm, fluid motion into turbulent alert spikes upon intrusion.

---

## 🚀 Key Features

- **Multi-Port Decoy Matrix:** Listens simultaneously on classic reconnaissance targets:
  - `TCP 23` — Telnet / IoT Gateway emulation with BusyBox banner.
  - `TCP 2222` — Alternative SSH OpenSSH banner trap.
  - `TCP 80` — HTTP Admin Gateway returning a realistic `401 Unauthorized` JSON payload.

- **Generative Hardware Visualizer:** Drives an organic, non-repeating dual-superposition sine wave across the 12×8 LED matrix, instantly morphing into an alert state when tripped.

- **Instant SIEM Telemetry:** Captures remote attacker IP addresses, timestamps, target ports, and raw TCP probe payloads, streaming structured logs directly over USB Serial.

- **Zero-External-Dependencies:** Runs using only core Arduino libraries:
  - `WiFiS3.h`
  - `Arduino_LED_Matrix.h`

---

## 🛠️ Hardware Requirements

- **Microcontroller:** Arduino Uno R4 WiFi, leveraging the onboard ESP32-S3 Wi-Fi coprocessor and Renesas RA4M1 main MCU.
- **Connection:** Standard USB-C cable for power and serial logging.
- **Network:** A local Wi-Fi network shared by the Arduino and authorized testing devices.

---

## ⚙️ Quick Start & Installation

### 1. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/r4honeypot.git
cd r4honeypot
```

### 2. Open the Firmware

Open `firmware/canary_node.ino` in the **Arduino IDE**.

### 3. Select Your Board

In the Arduino IDE, select:

**Tools → Board → Arduino UNO R4 WiFi**

### 4. Configure Wi-Fi Credentials

Update the configuration block in your Arduino sketch:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

Replace the placeholders with your local Wi-Fi credentials. Do not commit real credentials to a public repository.

### 5. Compile and Upload

Connect the Arduino to your computer, compile the sketch, and upload it to the board.

### 6. Monitor Live Telemetry

Open the **Serial Monitor** and set the baud rate to `115200`.

Monitor connection events, tripwire alerts, timestamps, remote IP addresses, and captured probe payloads.

---

## 📊 State Machine & Architecture

```text
[ Normal State ]
       |
       └──> Smooth Generative Wave
                    |
                    └──> Waiting on Local Subnet
                                  |
                                  v
                      Attacker / Scanner Hits Decoy Port
                                  |
                                  v
                            [ Tripwire Hit ]
                                  |
                                  ├──> Deception Banner Served
                                  |
                                  ├──> Attacker IP & Payload Logged
                                  |
                                  └──> LED Matrix Shifts to
                                       [ TURBULENCE ALERT ]
                                               |
                                               └──> 8 Seconds
```

---

## 🧪 Testing the Tripwire

Use the companion Python test script, `trigger_canary.py`, to simulate an authorized scan and trigger your hardware node.

### Python Test Script

```python
import socket

TARGET_IP = "YOUR_ARDUINO_IP"

ports_to_test = [
    (80, "HTTP-ADMIN-TRAP", b"GET /admin-panel HTTP/1.1\r\n\r\n"),
    (2222, "SSH-BANNER-TRAP", b"SSH-2.0-OpenSSH_Probe\r\n"),
    (23, "TELNET-IOT-TRAP", b"root\r\n"),
]

for port, service, payload in ports_to_test:
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(2.0)
            s.connect((TARGET_IP, port))
            s.sendall(payload)

            response = s.recv(1024)
            print(
                f"[+] Hit {service}: "
                f"{response.decode(errors='replace').strip()}"
            )

    except Exception as e:
        print(f"[!] Error on port {port}: {e}")
```

### Running the Test

1. Replace `YOUR_ARDUINO_IP` with the Arduino's assigned local IP address.
2. Ensure your testing machine and Arduino are connected to the same network.
3. Upload the firmware and confirm that the Arduino has connected to Wi-Fi.
4. Run the Python script:

   ```bash
   python trigger_canary.py
   ```

5. Check the Arduino Serial Monitor for corresponding tripwire events and observe the LED matrix alert.

**Note:** This script assumes the firmware accepts TCP connections on all three ports and sends a response after receiving the probe. The actual behavior depends on the implementation of `canary_node.ino`.

---

## 📁 Repository Structure

```text
r4honeypot/
├── firmware/
│   └── canary_node.ino
├── trigger_canary.py
├── LICENSE
├── README.md
└── .gitignore
```

---

## 🔒 Security Considerations

- Deploy the honeypot only on networks you own or are authorized to monitor.
- Keep the device isolated from sensitive production systems.
- Never commit Wi-Fi passwords or other secrets to version control.
- TCP payload logging may capture sensitive information. Restrict access to serial logs and handle collected data responsibly.
- The Arduino Uno R4 WiFi has limited memory and processing capacity. Keep payload buffers bounded and avoid excessive logging.
- Verify that each decoy service actually listens on its advertised port. The README describes the intended behavior, not a verified firmware implementation.

---

## 📜 License

Distributed under the **MIT License**. See the [`LICENSE`](LICENSE) file for more information.
