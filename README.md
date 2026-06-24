# 🦯 Blind Stick — Smart Navigation & Fall Detection Aid

An IoT-enabled smart cane for visually impaired users. It combines **ultrasonic obstacle detection**, **MPU6050-based fall detection**, and a **Firebase-connected mobile app** to alert when an obstacle is near or when the user has fallen.

## 🎯 What It Does

- Detects nearby obstacles using an ultrasonic sensor and warns the user via buzzer and/or phone vibration
- Continuously monitors motion using an MPU6050 accelerometer/gyroscope to detect falls
- Pushes obstacle and fall alerts to Firebase Realtime Database, picked up by a companion Android app
- Supports three alert modes — buzzer only, phone only, or both — switchable from the app

## 🌟 Key Features

| Feature | How |
|---|---|
| **Obstacle detection** | HC-SR04 ultrasonic sensor (`trigPin`/`echoPin`), distance thresholded per mode (70 cm / 120 cm) |
| **Three alert modes** | `mode = "11"` (both), `"1"` (buzzer only), else (phone only) — read live from `/Finder/mode` in Firebase |
| **Fall detection** | Dual-stage trigger logic on MPU6050 acceleration magnitude + rotation, similar to the classic 3-trigger fall-detection algorithm |
| **Cloud connectivity** | ESP8266 connects to WiFi and streams status to Firebase Realtime Database (`/Finder/...`) |
| **Companion app** | Android app (`Finder.apk`) reads alerts and lets the user switch modes remotely |

## 🧠 How Fall Detection Works

The firmware runs **two parallel fall-detection state machines** (legacy + refined), both built on the same idea — a 3-stage trigger sequence over a short time window:

1. **Trigger 1** — acceleration magnitude drops below a lower threshold (free-fall onset)
2. **Trigger 2** — acceleration magnitude spikes above an upper threshold shortly after (impact)
3. **Trigger 3** — orientation (rotation magnitude from gyroscope) changes significantly, then **settles** into a new stable orientation (lying down) within a timeout window

If all three stages fire within their respective timing windows, `Firebase.setInt(firebaseData, "/Finder/falled", 1)` is sent — the app/caregiver gets notified. The alert auto-clears after 15 seconds (`currentTime - previousTime > 15000`) if not re-triggered.

```
Trigger1 (AM drops)  →  Trigger2 (AM spikes)  →  Trigger3 (rotation settles)  →  FALL DETECTED
     ≤6 ticks timeout        ≤6 ticks timeout            ≤10 ticks confirm window
```

## 📂 Project Structure

```
Blind_Stick_project/
├── README.md
├── Application/
│   └── Finder.apk                          # Companion Android app (mode switch + alerts)
└── ugc_final_of_blind_stick/
    └── ugc_final_of_blind_stick.ino        # ESP8266 firmware: ultrasonic + MPU6050 + Firebase
```

## 🔧 Hardware Required

| Part | Role |
|---|---|
| ESP8266 (NodeMCU or similar) | Main controller, WiFi + Firebase connectivity |
| HC-SR04 ultrasonic sensor | Obstacle distance measurement |
| MPU6050 (×2 in code: `Adafruit_MPU6050` lib + raw I2C at `0x69`) | Acceleration + gyroscope for fall detection |
| Buzzer | Local audible alert |
| Android phone | Runs `Finder.apk` for remote alerts + mode control |

### Pin Mapping (ESP8266)

| Signal | Pin |
|---|---|
| Ultrasonic Trigger | `D5` |
| Ultrasonic Echo | `D7` |
| Buzzer | `D6` |

## 🚀 Getting Started

### 1. Flash the firmware

Open `ugc_final_of_blind_stick/ugc_final_of_blind_stick.ino` in the Arduino IDE.

**Install libraries:**
- `Adafruit_MPU6050`
- `Adafruit_Sensor`
- `FirebaseESP8266`
- `ESP8266WiFi`

**Configure before flashing:**
```cpp
#define FIREBASE_HOST "<your-project>.firebaseio.com"
#define FIREBASE_AUTH "<your-firebase-secret>"
#define WIFI_SSID     "<your-wifi-ssid>"
#define WIFI_PASSWORD "<your-wifi-password>"
```

> ⚠️ **Security note:** the checked-in `.ino` file currently has real WiFi and Firebase credentials hardcoded. Treat any existing API keys as compromised, rotate them in the Firebase console, and replace the values above with your own (or load them from a private `secrets.h` that's excluded from version control) before deploying or sharing this repo further.

Select the correct ESP8266 board in Arduino IDE, then upload.

### 2. Install the companion app

Install `Application/Finder.apk` on an Android device (you may need to enable "install from unknown sources"). The app reads/writes the same Firebase Realtime Database paths as the firmware:

- `/Finder/mode` — `"11"` (both), `"1"` (buzzer only), other (phone only)
- `/Finder/OnSound` — phone-alert flag set by the firmware
- `/Finder/falled` — fall-detected flag

### 3. Use it

Mount the ultrasonic sensor and MPU6050 on a cane, power the ESP8266, and pair with the app. Switch alert modes from the app depending on the environment (e.g., buzzer mode in quiet outdoor spaces, phone mode where buzzer noise is disruptive).

## 🛠️ Tech Stack

| Layer | Tech |
|---|---|
| Firmware | C++ (Arduino framework, ESP8266) |
| Sensors | HC-SR04 (ultrasonic), MPU6050 (IMU) |
| Cloud | Firebase Realtime Database |
| Mobile app | Android (`Finder.apk`) |

## 📄 License

Academic / hackathon project — educational use.
