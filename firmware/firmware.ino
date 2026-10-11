#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define BUTTON_1_PIN 12
#define BUTTON_2_PIN 13

#define DEBOUNCE_DELAY 50
#define LONG_PRESS_TIME 800
#define DOUBLE_CLICK_TIME 300

const char* WIFI_SSID = "XXXXX";
const char* WIFI_PASS = "XXXXX";
const char* OPENSKY_URL = "https://opensky-network.org/api/states/all";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

enum AppState {
  STATE_MAIN_MENU,
  STATE_LIVE_FLIGHTS,
  STATE_SETTINGS,
  STATE_ACTIONS
};

AppState currentState = STATE_MAIN_MENU;

int menuIndex = 0;
const int TOTAL_MENU_ITEMS = 3;
const char* menuItems[] = {"1. Live Flights", "2. Query API", "3. System Settings"};

String flightCallsign = "N/A";
String flightCountry = "N/A";
float flightAlt = 0.0;

struct ButtonState {
  uint8_t pin;
  bool lastReading;
  bool stableState;
  unsigned long lastDebounceTime;
  unsigned long pressStartTime;
  unsigned long lastClickTime;
  uint8_t clickCount;
  bool longPressHandled;
};

ButtonState btn1 = {BUTTON_1_PIN, HIGH, HIGH, 0, 0, 0, 0, false};
ButtonState btn2 = {BUTTON_2_PIN, HIGH, HIGH, 0, 0, 0, 0, false};

void drawMenu();
void drawLiveFlights();
void drawSettings();
void drawActions();
void fetchOpenSkyData();
void handleButtonEvent(uint8_t btnId, uint8_t eventType);
void updateButton(ButtonState &btn, uint8_t btnId);

void setup() {
  Serial.begin(115200);

  pinMode(btn1.pin, INPUT_PULLUP);
  pinMode(btn2.pin, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println(F("SkyChain Tier 1"));
  display.setCursor(10, 35);
  display.println(F("Connecting WiFi..."));
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  display.clearDisplay();
  display.setCursor(10, 25);
  display.println(F("WiFi Connected!"));
  display.display();
  delay(1000);

  drawMenu();
}

void loop() {
  updateButton(btn1, 1);
  updateButton(btn2, 2);
}

void updateButton(ButtonState &btn, uint8_t btnId) {
  bool reading = digitalRead(btn.pin);
  unsigned long now = millis();

  if (reading != btn.lastReading) {
    btn.lastDebounceTime = now;
  }

  if ((now - btn.lastDebounceTime) > DEBOUNCE_DELAY) {
    if (reading != btn.stableState) {
      btn.stableState = reading;

      if (btn.stableState == LOW) {
        btn.pressStartTime = now;
        btn.longPressHandled = false;
      } else {
        if (!btn.longPressHandled) {
          btn.clickCount++;
          btn.lastClickTime = now;
        }
      }
    }
  }

  if (btn.stableState == LOW && !btn.longPressHandled) {
    if ((now - btn.pressStartTime) >= LONG_PRESS_TIME) {
      btn.longPressHandled = true;
      btn.clickCount = 0;
      handleButtonEvent(btnId, 3);
    }
  }

  if (btn.clickCount > 0 && (now - btn.lastClickTime) > DOUBLE_CLICK_TIME) {
    if (btn.clickCount == 1) {
      handleButtonEvent(btnId, 1);
    } else if (btn.clickCount >= 2) {
      handleButtonEvent(btnId, 2);
    }
    btn.clickCount = 0;
  }

  btn.lastReading = reading;
}

void fetchOpenSkyData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(OPENSKY_URL);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(16384);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        JsonArray states = doc["states"];
        if (!states.isNull() && states.size() > 0) {
          JsonArray firstAircraft = states[0];
          flightCallsign = firstAircraft[1].as<String>();
          flightCountry = firstAircraft[2].as<String>();
          flightAlt = firstAircraft[7].as<float>();
        }
      }
    }
    http.end();
  }
}

void handleButtonEvent(uint8_t btnId, uint8_t eventType) {
  if (btnId == 1) {
    if (eventType == 1) {
      if (currentState == STATE_MAIN_MENU) {
        menuIndex = (menuIndex + 1) % TOTAL_MENU_ITEMS;
        drawMenu();
      } else {
        currentState = STATE_MAIN_MENU;
        drawMenu();
      }
    } else if (eventType == 2) {
      if (currentState == STATE_MAIN_MENU) {
        if (menuIndex == 0) {
          currentState = STATE_LIVE_FLIGHTS;
          fetchOpenSkyData();
          drawLiveFlights();
        } else if (menuIndex == 1) {
          currentState = STATE_ACTIONS;
          fetchOpenSkyData();
          drawActions();
        } else if (menuIndex == 2) {
          currentState = STATE_SETTINGS;
          drawSettings();
        }
      }
    } else if (eventType == 3) {
      currentState = STATE_MAIN_MENU;
      drawMenu();
    }
  } 
  else if (btnId == 2) {
    if (eventType == 1) {
      if (currentState == STATE_LIVE_FLIGHTS) {
        fetchOpenSkyData();
        drawLiveFlights();
      }
    } else if (eventType == 2) {
      currentState = STATE_MAIN_MENU;
      drawMenu();
    } else if (eventType == 3) {
      currentState = STATE_SETTINGS;
      drawSettings();
    }
  }
}

void drawMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("=== SKYCHAIN TIER 1 ==="));
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  for (int i = 0; i < TOTAL_MENU_ITEMS; i++) {
    display.setCursor(10, 18 + (i * 14));
    if (i == menuIndex) {
      display.print(F("> "));
    } else {
      display.print(F("  "));
    }
    display.println(menuItems[i]);
  }

  display.display();
}

void drawLiveFlights() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("[ OPENSKY FLIGHTS ]"));
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.setCursor(0, 16);
  display.print(F("Flight: ")); display.println(flightCallsign);
  display.setCursor(0, 28);
  display.print(F("Origin: ")); display.println(flightCountry);
  display.setCursor(0, 40);
  display.print(F("Altitude: ")); display.print(flightAlt); display.println(F("m"));
  display.setCursor(0, 52);
  display.println(F("B2: Refresh | B1: Back"));

  display.display();
}

void drawActions() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("[ API QUERY STATUS ]"));
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.setCursor(0, 20);
  display.println(F("API Request Sent!"));
  display.setCursor(0, 36);
  display.print(F("Parsed Callsign: ")); display.println(flightCallsign);
  display.setCursor(0, 52);
  display.println(F("B1: Back to Menu"));

  display.display();
}

void drawSettings() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("[ SYSTEM CONFIG ]"));
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.setCursor(0, 16);
  display.println(F("SSID: XXXXX"));
  display.setCursor(0, 28);
  display.println(F("API: OpenSky REST"));
  display.setCursor(0, 40);
  display.println(F("Firmware: v2.0-API"));
  display.setCursor(0, 52);
  display.println(F("B1: Back to Menu"));

  display.display();
}
