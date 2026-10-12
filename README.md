# SIREN — WiFi Evil-Twin / Pentest Tool

<p align="center"><strong>P01yg107 Labs</strong> · ESP32-S2 Mini · I²C OLED · custom firmware</p>

> **Hand-built by a real hacker.** Designed, printed, flashed & tested by an active bug-bounty researcher and CTF author — not drop-shipped, not rebranded. Firmware is **ours, top to bottom** (Arduino / C++, MIT).

SIREN is a handheld Wi-Fi pentest unit built on an **ESP32-S2 Mini** with a live **SH1106 I²C OLED**. Power it from any USB port and drive it from the on-device screen or the built-in web dashboard at **http://192.168.4.1**. It maps the airspace on a live signal meter, stands up a look-alike AP + captive portal for authorized phishing-resistance testing, and shows captured results on its own screen in real time.

---

## Features

- **Live scan meter** — every nearby AP drawn as a real-time signal equalizer on the OLED, sorted strongest-first; web table adds vendor (OUI), channel, encryption, hidden flag.
- **Evil-twin AP** — clone a target SSID and re-spawn the access point under that name.
- **Captive / login portal** — serves a login page; captured credentials appear **live on the OLED** (with a "GOT CRED" flash) *and* in the web dashboard.
- **Device hunt** — lock onto a target BSSID and walk it down with a live RSSI locator bar.
- **Deauth** — targeted deauth + disassoc frames against a chosen BSSID/channel.
- **Beacon flood** — broadcast a list of fake SSIDs (or random names).
- **Targeted + continuous scanning** and **CSV export** of the last scan.
- **Two interfaces** — glanceable OLED home screen *and* a full web GUI. No app, no cloud.

---

## Hardware

| Part | Detail |
|------|--------|
| MCU | ESP32-S2 Mini (LOLIN S2 Mini / generic S2 Mini) |
| Display | 1.3" 128×64 I²C OLED, **SH1106** driver, addr `0x3C` |
| I²C pins | SDA = GPIO8, SCL = GPIO9 |
| Power | USB-C, 5 V |
| Enclosure | Two-shell PETG-CF case, flush Hyper-PLA red siren emblem (P01yg107 house style) |

---

## Build & Flash (Arduino IDE)

**Libraries:** `Adafruit GFX`, `Adafruit SH110X` (Library Manager).

**Board settings (Tools menu):**
- Board: **ESP32S2 Dev Module** (or *LOLIN S2 Mini*)
- **USB CDC On Boot: Enabled**  ← keeps the serial port alive so you don't lose COM every upload
- Upload speed: 460800

**One sketch per folder:** keep `P01yg107_SIREN.ino` alone in a folder named `P01yg107_SIREN`. Two `.ino` files in one folder = `redefinition of ...` errors.

**Download mode (S2 has native USB, no UART chip):** unplug → hold **`0` (BOOT)** → tap **RST** → release `0`. A new COM port appears in Device Manager; select it and upload.

---

## Usage

1. Power on → centered squid boot logo → home screen.
2. Join the AP **`P01yg107_Command`** (password `password123`).
3. Browse to **http://192.168.4.1** for the full dashboard, or read the OLED directly.

**OLED views** (precedence: Hunt → Portal → Scan meter), header `P01yg107 Labs`, `GUI 192.168.4.1` pinned at the bottom:
- **Scan meter** — AP equalizer with a sweeping scanner cursor.
- **Portal** — latest captured `U:` / `P:` and a capture count.
- **Hunt** — target tail + live RSSI bar.

---

## Troubleshooting

| Symptom | Cause / Fix |
|---------|-------------|
| `Could not open COMx / port busy` on upload | S2 dropped its USB port. Enter **download mode** (hold `0`, tap RST, release `0`), pick the new COM. Set **USB CDC On Boot: Enabled** so the port persists. |
| Nothing happens on a wall charger / battery | Board still auto-runs on 5 V. If dead only on a battery pack, it's the pack's **low-current auto-shutoff** — use a wall adapter or an "always-on" bank. |
| Random resets when beacon/deauth kicks on | **Brownout** from TX current on a weak cable/port. Use a real data cable + solid 5 V. Boot prints `reset reason` on serial to confirm. |
| Blank OLED but WiFi works | Display init now retries 5× then continues **headless** — check wiring (SDA 8 / SCL 9, addr 0x3C) but the node still boots. |
| `redefinition of 'display' ...` at compile | Two `.ino` in the sketch folder — keep only one. |
| Off-channel deauth drops your link to the GUI | Expected: one radio. The node snaps back to its AP channel on STOP. |

---

## ⚠️ Authorized use only

SIREN transmits real 802.11 management frames (deauth, beacon) and captures credentials on the captive portal. Use it **only** on networks and devices you own or are explicitly contracted to test. You are responsible for using it legally. For education and authorized security testing.

---

## License & credits

Firmware © 2026 Joshua Burton / **P01yg107 Labs** — released under the **MIT License** (see [`LICENSE`](LICENSE)). This firmware is original work.

Built with the open-source **Adafruit GFX** and **Adafruit SH110X** libraries (BSD/MIT; © Adafruit Industries).

🦑 Part of the P01yg107 Labs tool line · [p01ylabs.com](https://p01ylabs.com)
