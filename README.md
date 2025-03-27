# ESP32 Wi-Fi Provisioning + LED Control Interface

A fully interactive setup flow for provisioning your ESP32 device via a sleek captive portal and controlling multiple LEDs via either HTTP or serial commands. This project also features handshake logic to communicate with a Unity game, enabling real-time hardware feedback.

⚠️ Note: This project stems from work done for a startup prototype involving ESP32 device provisioning and Unity integration. The version in this repository is a representative example, built for open-source demonstration and stripped of proprietary elements or production-specific code.
---

## 🌟 Features

- Captive portal UI for initial Wi-Fi configuration
- Dynamic scanning and selection of available networks
- Stores SSID and password using EEPROM
- Supports LED control over:
  - HTTP requests (`/led?led=1&state=1`)
  - Serial commands (from Unity handshake)
- mDNS support: access the device via `http://esp32-led.local`
- Handshake listener to verify Unity connection (`dis u?` → `ya`)

---

## 🛠️ Requirements

- ESP32 board (tested on ESP32-WROOM-32)
- Arduino IDE
- Arduino libraries:
  - `WiFi.h`
  - `WebServer.h`
  - `DNSServer.h`
  - `EEPROM.h`
  - `ESPmDNS.h`

---

## 🌐 Captive Portal UI

The HTML, CSS, and JavaScript are embedded in the sketch via raw string literal. This UI:

- Greets the user with a fun onboarding screen
- Lets them scan nearby Wi-Fi networks
- Allows SSID/password input
- Animates the connection process

> Note: the UI auto-redirects all HTTP traffic through DNS spoofing via the AP.

---

## 🚀 Getting Started

### 1. Flash to ESP32
- Rename the file to `.ino` if it's still `.cpp`
- Open with Arduino IDE
- Install the necessary libraries (see above)
- Connect your ESP32 via USB
- Select correct port and board from **Tools** menu
- Upload!

### 2. Connect to Captive Portal
- Your ESP32 will broadcast a network: `ESP32_Provisioning`
- Password: `12345678`
- Connect from any device and open a browser. It should redirect automatically.

### 3. Enter Wi-Fi Credentials
- Choose a network from the scan list
- Input password
- Click **Connect**
- If successful, you'll see a confirmation + `Done` button

---

## 🛡️ LED Control API

### Over HTTP
- `GET /led?led=1&state=1` → turns **LED 1** ON
- `GET /led?led=2&state=0` → turns **LED 2** OFF

### Over Serial (from Unity or Serial Monitor)
- Format: `1:1` or `2:0` (LED #: state)
- Send `dis u?` to trigger handshake → ESP32 replies `ya`

---

## 💡 Tip: Unity Integration

To pair with Unity:
- Use `SerialPort.WriteLine("dis u?")` to confirm ESP32 presence
- Then use `SerialPort.WriteLine("1:1")` to control LED 1
- Ensure Unity serial port settings match ESP32 baud rate: `115200`

> Without the Unity-side handshake, the device won't respond over serial.

---

## 🚩 Troubleshooting

- Can't connect to `esp32-led.local`? Try the IP shown in serial monitor
- If Wi-Fi fails, reflash or add logic to reset EEPROM
- Using a different ESP32? Pin mappings may need updating

---

## 🚀 Demo Preview (optional)
Want to see the interface in action? Add screenshots or a video/gif here:
```
![Captive UI Screenshot](assets/wifiprovisioning.gif)
![LED Control Success](assets/breadboardworking.gif)
```

---

## 🎓 Credits

Designed and built with love by [meeee](https://github.com/ndrada) ✨

