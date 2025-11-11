# ESP8266 Weather‑Aware Smart Lighting System

Automatically adjust lighting brightness based on real‑time sunlight data, smooth sunrise/sunset transitions, and remote commands via Telegram. Built for ESP8266 with Wi‑Fi, Open‑Meteo, and a Telegram bot.

---

## Why I Made This

This whole build began from a simple truth: I’m lazy in the garden. My live plant bowl looked wonderful, but remembering to tweak light cycles, check timing, and worry about day–night patterns felt like chore‑mode. So the goal became elegant automation-let the microcontroller keep the plants happy while I sip tea and forget I even own them.

The result: a small but capable system that handles brightness, reacts to daylight, and already hints at future upgrades like water level and temperature management.

## Overview

This project implements an **ESP8266‑based automated lighting controller** that can run in three main modes:

* **Weather Mode** - Dynamic PWM brightness from current shortwave solar radiation; gentle sunrise/sunset ramps.
* **Full Brightness** - Hard 100% output.
* **Custom Brightness** - User‑selected level (0–100%) with EEPROM persistence.

It also includes:

* Wi‑Fi auto connect/reconnect
* Telegram bot control with an allowlist
* Periodic weather polling
* Smooth transitions (LERP) for both brightness and day/night changes
* Human‑readable status via bot (brightness, radiation, sunrise, sunset, time)

---

## Features

* 🔗 **Robust Wi‑Fi:** Station mode, hostname set, auto‑reconnect, persistent config.
* 🌤 **Weather‑aware:** Pulls current `shortwave_radiation` plus today’s `sunrise`/`sunset` from Open‑Meteo.
* 🌅 **Sunrise/Sunset ramps:** Time‑windowed LERP for natural transitions.
* 📩 **Telegram control:** Commands and inline keyboard for mode switching and brightness setting.
* 💾 **EEPROM storage:** Remembers last mode and custom brightness across reboots.
* ⏱️ **Tuned timing:** API every 10 mins, bot poll every 500 ms, loop tick ~20 ms.

---

## Repository Layout

```
project/
├── main.cpp                # Core runtime: setup/loop, modes, transitions
├── WifiModule.cpp          # Wi‑Fi helpers (connect, status)
├── Weather_API.cpp         # API fetch & parsing; time utilities
├── Telegram.cpp            # Telegram bot: commands, keyboard, allowlist
├── config.cpp              # Globals: clients, bot, intervals, constants
├── credentials.cpp         # SSID/PASS, BOT token, API endpoint (replace!)
└── include/                # *.h headers (expected)
```

> **Note:** Move secrets out of source before publishing (see Security).

---

## Hardware Requirements

* **ESP8266** (e.g., NodeMCU, Wemos D1 mini)
* **LED / Driver** capable of PWM control (e.g., MOSFET + LED strip)
* Stable **5V supply** sized for the LED load
* Wiring for **PWM output** from the chosen pin to the driver/LED

> Ensure proper common ground between ESP8266 and the LED driver.

---

## Software Requirements

* Arduino IDE or PlatformIO with **ESP8266 core** installed
* Libraries:

  * `ESP8266WiFi`
  * `WiFiClientSecure`
  * `HTTPClient`
  * `EEPROM`
  * `Arduino_JSON`
  * `UniversalTelegramBot`

---

## Configuration

### 1) Credentials

Create `` and set:

```cpp
const char* SSID = "<your-ssid>";
const char* PASS = "<your-password>";
const char* BOT_API = "<telegram-bot-token>";
const char* WAPI = "https://api.open-meteo.com/v1/forecast?..."; // with latitude, longitude, and fields
```

* Use your **latitude/longitude** and include fields: `current=shortwave_radiation` and `daily=sunrise,sunset` with `timezone=auto` and `forecast_days=1`.
* For production, consider storing secrets in a separate file excluded by `.gitignore`.

### 2) Allowed Chat IDs

In **`config.cpp`**, set the `allowedChatId[]` array to the Telegram user IDs permitted to control the device.

### 3) Pins

* Ensure `LED_TEST` and any other pins referenced in headers match your wiring (e.g., D5 for PWM).

---

## Build & Flash

### Arduino IDE

1. Install **ESP8266 Boards** via Boards Manager.
2. Install required libraries.
3. Open the project, select your ESP8266 board and COM port.
4. Compile and upload.
5. Open **Serial Monitor** at **115200** baud.

### PlatformIO (alternative)

* Create a new ESP8266 project or add a `platformio.ini`.
* List libraries in `lib_deps` and build/upload.

---

## Operation

### Boot Sequence (simplified)

1. Initialize Serial and EEPROM.
2. Configure and connect Wi‑Fi (station mode; hostname set).
3. Start TLS client in insecure mode (for Open‑Meteo HTTPS; see Security).
4. Sync time via NTP (offset +19800s for IST).
5. **Initial weather fetch** to populate sunrise/sunset and radiation.
6. Restore mode and brightness from EEPROM (if custom mode was active).

### Runtime Loop

* **Telegram Polling** every 500 ms → read new messages → handle commands.
* **Weather Polling** every 10 minutes → update `sunrise`, `sunset`, `radiation`.
* **Mode Engine** executes one of: Weather / Full / Custom / Off.
* **PWM Output** updated with smoothing (LERP / easing) to prevent flicker.

---

## Modes & Persistence

| Mode                | EEPROM code | Behavior                                   |
| ------------------- | ----------- | ------------------------------------------ |
| **OFF**             | 0           | Output off                                 |
| **Weather**         | 1           | Radiation‑driven with sunrise/sunset ramps |
| **Full Brightness** | 2           | 100% duty                                  |
| **Custom**          | 3           | 0–100% user value; smoothed PWM            |

* Address **0** stores the mode.
* Address **1** stores the custom brightness (0–100).

---

## Weather Logic Details

* Fetches **`current.shortwave_radiation`** and today’s **`daily.sunrise`********/****************************************`daily.sunset`**.
* Converts times to **minutes since midnight**.
* Determines **state machine**: `SUNRISE` → `DAY` → `SUNSET` → `NIGHT`.
* Uses **LERP** windows (e.g., ~25–30 minutes) for gentle changes.
* Daytime tracks radiation with a small smoothing factor; night decays to zero.

### Key Time Helpers

* `currentTime()` → returns minutes [0..1439]
* `todayDay()` → day‑of‑year for once‑per‑day transitions

---

## Telegram Control

### Basic Commands

* `/start` → Shows keyboard.
* `weather mode` → Enables dynamic weather mode.
* `full brightness` → Forces 100% duty.
* `off` → Turns output off.
* `status` or `/status` → Sends current values (brightness %, radiation, sunrise, sunset, current time).
* `0`–`100` → Sets custom brightness and saves to EEPROM.

> Only messages from **allowed chat IDs** are honored. Others are rejected.

### Status Example

```
*Current Status*
• Brightness: 82%
• Sun Radiation: 200
• Sunrise: 06:42
• Sunset: 18:10
• Current Time: 14:22
```

---

## Timing Parameters (defaults)

* **Weather API interval:** 600,000 ms (10 min)
* **Telegram poll interval:** 500 ms
* **Loop delay:** ~20 ms
* **Sunrise ramp window:** ~25 min
* **Sunset ramp window:** ~30 min

You can adjust these in the respective source files.

---

## Wiring Notes

* Connect the PWM pin (e.g., `LED_TEST`) to a MOSFET gate via a **gate resistor**; source to GND; drain to LED‑ strip negative; LED‑strip positive to supply.
* Use a **common ground** between ESP8266 and the LED power supply.
* Add a **flyback diode** for inductive loads (not needed for purely resistive LED strips with proper driver).
* Ensure the LED current does not exceed the MOSFET and power supply ratings.


## Troubleshooting

* **Wi‑Fi won’t connect:** Check SSID/PASS, antenna, distance, and power stability.
* **Bot unresponsive:** Verify token, ensure device has internet; confirm your chat ID is in the allowlist.
* **Choppy brightness:** Increase smoothing (alpha) or loop delay; ensure PWM pin supports `analogWrite` on your board.
* **Wrong times:** Confirm NTP sync and timezone; ensure Open‑Meteo request has `timezone=auto`.

---

## Roadmap

Upcoming enhancements planned for the Live Plant Bowl deployment:

* Temperature sensing & auto‑cooling/ventilation control
* Water level monitoring with refill/pump automation
* Soil moisture sensing + growth‑friendly watering schedule
* Adaptive seasonal lighting (longer winter days, shorter summer nights)
* MQTT / Home Assistant integration
* Local fallback mode (operate without internet)
* OTA firmware updates
* Web UI for configuration & live telemetry
* Data logging & growth‑stage analytics
* AI‑assisted plant‑health suggestions

---

## License

MIT License. See `LICENSE` if included; otherwise treat code as MIT by default.

---