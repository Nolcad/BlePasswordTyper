#include <Arduino.h>
#include <BleKeyboard.h>
#include <M5GFX.h>
#include <M5StickC.h>
#include <NimBLEDevice.h>
#include <Preferences.h>

#include "AXP192.h"

const char* PasswordSequence = ".\\USER\tPASSWORD\n";

Preferences preferences;

BleKeyboard bleKeyboard("PasswordTyper", "Nolcad", 100);

M5GFX display;
M5Canvas canvas;

unsigned long now;
// Screen update in ms
#define SCREEN_UPDATE_PERIOD 100
unsigned long lastScreenUpdate;

// Power management
enum class BatteryState : uint8_t { empty, discharging, charging, full, standby };
BatteryState batteryState;

#define POWER_OFF_TIMEOUT (30 * 1000L)
unsigned long lastPowerOffTimeoutReset;

#define POWER_UPDATE_PERIOD 1000
unsigned long lastPowerUpdate;

enum class BatteryGaugeState : uint8_t { idle, lowBoundary, highBoundary };
BatteryGaugeState batteryGaugeState = BatteryGaugeState::idle;

uint8_t batteryCapacity = 0;
uint8_t batterySoc = 0;
#define FULL_CHARGE_TIME_BEFORE_GAUGE_RESET (30 * 1000)
unsigned long batteryFullChargeTime;

#define LED_SLOW_BLINK_PERIOD 1000
#define LED_FAST_BLINK_PERIOD 250
enum class LedLightMode : uint8_t { off, on, slowBlink, fastBlink };
LedLightMode ledLightMode = LedLightMode::off;
bool ledState = false;
unsigned long LastLedUpdate = 0;

void ledUpdate(LedLightMode lightMode) {
  unsigned long now = millis();
  switch (lightMode) {
  case LedLightMode::off:
    ledState = false;
    LastLedUpdate = now;
    break;

  case LedLightMode::on:
    ledState = true;
    LastLedUpdate = now;
    break;

  case LedLightMode::slowBlink:
    if (now - LastLedUpdate >= LED_SLOW_BLINK_PERIOD) {
      ledState = !ledState;
      LastLedUpdate = now;
    }
    break;

  case LedLightMode::fastBlink:
    if (now - LastLedUpdate >= LED_FAST_BLINK_PERIOD) {
      ledState = !ledState;
      LastLedUpdate = now;
    }
    break;
  }
  if (ledState) {
    digitalWrite(10, LOW);
  } else {
    digitalWrite(10, HIGH);
  }
}

void setup() {
  display.setBrightness(0);
  display.begin();
  display.setRotation(3);
  display.setFont(&fonts::Font8x8C64);
  canvas.createSprite(display.width(), display.height());
  canvas.printf("Starting...");
  canvas.pushSprite(&display, 0, 0);

  M5.begin(false, true, true);
  M5.Axp.SetChargeCurrent(CURRENT_100MA);
  M5.Axp.EnableCoulombcounter();
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  preferences.begin("BleKeyboard", true);
  batteryCapacity = preferences.getUChar("BatteryCapacity", 80);
  batteryGaugeState = static_cast<BatteryGaugeState>(preferences.getUChar("GaugeState", 0));
  preferences.end();
  log_i("Restored battery capacity from NVS: %u", batteryCapacity);
  log_i("Restored gauge state from NVS: %u", batteryGaugeState);

  bleKeyboard.begin();
}

void loop() {
  now = millis();
  M5.update();
  ledUpdate(ledLightMode);

  if (now - lastPowerUpdate >= POWER_UPDATE_PERIOD) {
    lastPowerUpdate = now;

    bool isEmpty = M5.Axp.GetWarningLevel();
    bool isCharging = M5.Axp.GetBatteryChargingStatus() & 0x40;
    bool isPowered = M5.Axp.GetInputPowerStatus() & 0x20;

    ledLightMode = LedLightMode::off;

    if (!isPowered && !isCharging) {
      batteryState = BatteryState::discharging;
    }

    if (!isPowered && isCharging) {
      batteryState = BatteryState::charging;
      ledLightMode = LedLightMode::slowBlink;
    }

    if (isPowered && isCharging) {
      batteryState = BatteryState::charging;
      ledLightMode = LedLightMode::slowBlink;
    }

    BatteryState oldBatteryState = batteryState;
    if (isPowered && !isCharging) {
      batteryState = BatteryState::standby;
      if (oldBatteryState == BatteryState::standby &&
          batteryFullChargeTime < FULL_CHARGE_TIME_BEFORE_GAUGE_RESET) {
        batteryFullChargeTime += POWER_UPDATE_PERIOD;
      }
      if (batteryFullChargeTime >= FULL_CHARGE_TIME_BEFORE_GAUGE_RESET) {
        // Here we reached the high boundary.
        // Update capacity if we went through low boundary first
        if (batteryGaugeState == BatteryGaugeState::lowBoundary) {
          batteryGaugeState = BatteryGaugeState::idle;
          batteryCapacity = M5.Axp.GetCoulombData();
          log_i("Saving battery capacity in NVS: %u", batteryCapacity);
          log_i("Saving battery gauge state in NVS: %u", batteryGaugeState);
          preferences.begin("BleKeyboard", false);
          preferences.putUChar("BatteryCapacity", batteryCapacity);
          preferences.putUChar("GaugeState", static_cast<uint8_t>(batteryGaugeState));
          preferences.end();
        }
        batterySoc = 100;
        batteryState = BatteryState::full;
      }
    } else {
      batteryFullChargeTime = 0;
    }

    if (isEmpty) {
      ledLightMode = LedLightMode::fastBlink;
      // Here we reached the low boundary
      batteryGaugeState = BatteryGaugeState::lowBoundary;
      log_i("Battery is nearly empty, storing gauge state in NVS: %u", batteryGaugeState);
      preferences.begin("BleKeyboard", false);
      preferences.putUChar("GaugeState", static_cast<uint8_t>(batteryGaugeState));
      preferences.end();
      M5.Axp.ClearCoulombcounter();
      batteryState = BatteryState::empty;
    }

    if (M5.Axp.GetCoulombData() >= batteryCapacity || batteryState == BatteryState::full) {
      batterySoc = 100;
    } else if (M5.Axp.GetCoulombData() <= 0) {
      batterySoc = 0;
    } else {
      batterySoc = M5.Axp.GetCoulombData() * 100 / batteryCapacity;
    }

    bleKeyboard.setBatteryLevel(batterySoc);

    log_i("Bat Status: 0x%02X, PowerStatus: 0x%02X, WarningLevel: %d, "
          "BatVoltage: %0.3f",
          M5.Axp.GetBatteryChargingStatus(), M5.Axp.GetInputPowerStatus(), M5.Axp.GetWarningLevel(),
          M5.Axp.GetBatVoltage());
  }

  if (now - lastScreenUpdate >= SCREEN_UPDATE_PERIOD) {
    lastScreenUpdate = now;
    canvas.clear();
    canvas.setCursor(0, 0);

    canvas.setTextColor(TFT_WHITE);
    canvas.printf("BLE:           ");
    if (bleKeyboard.isConnected()) {
      canvas.setTextColor(TFT_GREEN);
    } else {
      canvas.setTextColor(TFT_DARKGRAY);
    }
    canvas.printf("OK\r\n");

    canvas.setTextColor(TFT_WHITE);
    canvas.printf(" - Keyboard:   ");
    if (bleKeyboard.isKeyboardSubscribed()) {
      canvas.setTextColor(TFT_GREEN);
    } else {
      canvas.setTextColor(TFT_DARKGRAY);
    }
    canvas.printf("OK\r\n");

    canvas.setTextColor(TFT_WHITE);
    canvas.printf(" - Media keys: ");
    if (bleKeyboard.isMediaKeysSubscribed()) {
      canvas.setTextColor(TFT_GREEN);
    } else {
      canvas.setTextColor(TFT_DARKGRAY);
    }
    canvas.printf("OK\r\n");

    canvas.setTextColor(TFT_WHITE);
    canvas.printf(" - LED:        ");
    if (bleKeyboard.getLedState() != 0xFF) {
      canvas.setTextColor(bleKeyboard.getLedState() & LED_NUM_LOCK ? TFT_GREEN : TFT_DARKGREY);
      canvas.printf("N ");
      canvas.setTextColor(bleKeyboard.getLedState() & LED_CAPS_LOCK ? TFT_GREEN : TFT_DARKGREY);
      canvas.printf("C ");
      canvas.setTextColor(bleKeyboard.getLedState() & LED_SCROLL_LOCK ? TFT_GREEN : TFT_DARKGREY);
      canvas.printf("S ");
    } else {
      canvas.setTextColor(TFT_DARKGRAY);
      canvas.printf("- - - ");
    }

    canvas.printf("\r\n\r\n");

    canvas.setTextColor(TFT_WHITE);
    canvas.printf("Battery:       ");
    canvas.printf("%3.2f mA.h", M5.Axp.GetCoulombData());

    canvas.printf("\r\n");
    canvas.printf(" - State:      ");
    switch (batteryState) {
    case BatteryState::empty:
      canvas.setTextColor(TFT_RED);
      canvas.printf("Empty");
      canvas.setTextColor(TFT_WHITE);
      break;

    case BatteryState::discharging:
      canvas.setTextColor(TFT_ORANGE);
      canvas.printf("Discharging");
      canvas.setTextColor(TFT_WHITE);
      break;

    case BatteryState::charging:
      canvas.setTextColor(TFT_GREEN);
      canvas.printf("Charging");
      canvas.setTextColor(TFT_WHITE);
      break;

    case BatteryState::full:
      canvas.setTextColor(TFT_GREEN);
      canvas.printf("Full");
      canvas.setTextColor(TFT_WHITE);
      break;

    case BatteryState::standby:
      canvas.printf("Standby");
      break;
    }
    canvas.printf("\r\n");
    canvas.printf(" - SoC:        ");
    canvas.printf("%u %%", batterySoc);

    /*
        canvas.printf(
            "Bat Status: 0x%02X, PowerStatus: 0x%02X, WarningLevel: %d, "
            "BatVoltage: %0.3f\r\n",
            M5.Axp.GetBatteryChargingStatus(), M5.Axp.GetInputPowerStatus(),
            M5.Axp.GetWarningLevel(), M5.Axp.GetBatVoltage());
    */

    unsigned long timeoutElapsed = (now - lastPowerOffTimeoutReset) * 154 / POWER_OFF_TIMEOUT;
    canvas.drawRect(2, 73, 156, 6, TFT_DARKGRAY);
    canvas.drawRect(4, 75, 154 - timeoutElapsed, 2, TFT_DARKGRAY);
    canvas.pushSprite(&display, 0, 0);
  }

  if (now - lastPowerOffTimeoutReset >= POWER_OFF_TIMEOUT) {
    M5.Axp.PowerOff();
  }
  // 0x01 long press(1s), 0x02 press
  if (M5.Axp.GetBtnPress() == 0x02) {
    M5.Axp.PowerOff();
  }

  if (M5.BtnA.wasReleased()) {
    lastPowerOffTimeoutReset = now;
    log_i("Button pressed");
    if (bleKeyboard.isConnected()) {
      log_i("Sending data");

      bleKeyboard.printf(PasswordSequence);

      log_i("Data sent");
    }
  }

  if (M5.BtnB.wasReleased()) {
    lastPowerOffTimeoutReset = now;
    bleKeyboard.pressKey(KEY_MEDIA_MUTE);
    bleKeyboard.releaseKey(KEY_MEDIA_MUTE);
  }

  switch (batteryState) {
  case BatteryState::empty:
  case BatteryState::discharging:
  case BatteryState::charging:
    break;
  case BatteryState::full:
  case BatteryState::standby:
    lastPowerOffTimeoutReset = now;
    break;
  }
}
