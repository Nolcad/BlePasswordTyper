#ifndef BLEKEYBOARD_H
#define BLEKEYBOARD_H

#include "NimBLEHIDDevice.h"

#include "Print.h"

const uint8_t KEY_LEFT_CTRL = 0x80;
const uint8_t KEY_LEFT_SHIFT = 0x81;
const uint8_t KEY_LEFT_ALT = 0x82;
const uint8_t KEY_LEFT_GUI = 0x83;
const uint8_t KEY_RIGHT_CTRL = 0x84;
const uint8_t KEY_RIGHT_SHIFT = 0x85;
const uint8_t KEY_RIGHT_ALT = 0x86;
const uint8_t KEY_RIGHT_GUI = 0x87;

const uint8_t KEY_UP_ARROW = 0xDA;
const uint8_t KEY_DOWN_ARROW = 0xD9;
const uint8_t KEY_LEFT_ARROW = 0xD8;
const uint8_t KEY_RIGHT_ARROW = 0xD7;
const uint8_t KEY_BACKSPACE = 0xB2;
const uint8_t KEY_TAB = 0xB3;
const uint8_t KEY_RETURN = 0xB0;
const uint8_t KEY_ESC = 0xB1;
const uint8_t KEY_INSERT = 0xD1;
const uint8_t KEY_PRTSC = 0xCE;
const uint8_t KEY_DELETE = 0xD4;
const uint8_t KEY_PAGE_UP = 0xD3;
const uint8_t KEY_PAGE_DOWN = 0xD6;
const uint8_t KEY_HOME = 0xD2;
const uint8_t KEY_END = 0xD5;
const uint8_t KEY_CAPS_LOCK = 0xC1;
const uint8_t KEY_F1 = 0xC2;
const uint8_t KEY_F2 = 0xC3;
const uint8_t KEY_F3 = 0xC4;
const uint8_t KEY_F4 = 0xC5;
const uint8_t KEY_F5 = 0xC6;
const uint8_t KEY_F6 = 0xC7;
const uint8_t KEY_F7 = 0xC8;
const uint8_t KEY_F8 = 0xC9;
const uint8_t KEY_F9 = 0xCA;
const uint8_t KEY_F10 = 0xCB;
const uint8_t KEY_F11 = 0xCC;
const uint8_t KEY_F12 = 0xCD;
const uint8_t KEY_F13 = 0xF0;
const uint8_t KEY_F14 = 0xF1;
const uint8_t KEY_F15 = 0xF2;
const uint8_t KEY_F16 = 0xF3;
const uint8_t KEY_F17 = 0xF4;
const uint8_t KEY_F18 = 0xF5;
const uint8_t KEY_F19 = 0xF6;
const uint8_t KEY_F20 = 0xF7;
const uint8_t KEY_F21 = 0xF8;
const uint8_t KEY_F22 = 0xF9;
const uint8_t KEY_F23 = 0xFA;
const uint8_t KEY_F24 = 0xFB;

const uint8_t KEY_NUM_0 = 0xEA;
const uint8_t KEY_NUM_1 = 0xE1;
const uint8_t KEY_NUM_2 = 0xE2;
const uint8_t KEY_NUM_3 = 0xE3;
const uint8_t KEY_NUM_4 = 0xE4;
const uint8_t KEY_NUM_5 = 0xE5;
const uint8_t KEY_NUM_6 = 0xE6;
const uint8_t KEY_NUM_7 = 0xE7;
const uint8_t KEY_NUM_8 = 0xE8;
const uint8_t KEY_NUM_9 = 0xE9;
const uint8_t KEY_NUM_SLASH = 0xDC;
const uint8_t KEY_NUM_ASTERISK = 0xDD;
const uint8_t KEY_NUM_MINUS = 0xDE;
const uint8_t KEY_NUM_PLUS = 0xDF;
const uint8_t KEY_NUM_ENTER = 0xE0;
const uint8_t KEY_NUM_PERIOD = 0xEB;

const uint8_t LED_NUM_LOCK = 0x01;
const uint8_t LED_CAPS_LOCK = 0x02;
const uint8_t LED_SCROLL_LOCK = 0x04;
const uint8_t LED_COMPOSE = 0x08;
const uint8_t LED_KANA = 0x10;

typedef uint8_t MediaKeyReport[2];

const MediaKeyReport KEY_MEDIA_NEXT_TRACK = {1, 0};
const MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK = {2, 0};
const MediaKeyReport KEY_MEDIA_STOP = {4, 0};
const MediaKeyReport KEY_MEDIA_PLAY_PAUSE = {8, 0};
const MediaKeyReport KEY_MEDIA_MUTE = {16, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_UP = {32, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_DOWN = {64, 0};
const MediaKeyReport KEY_MEDIA_WWW_HOME = {128, 0};
const MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER = {0, 1};
const MediaKeyReport KEY_MEDIA_CALCULATOR = {0, 2};
const MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS = {0, 4};
const MediaKeyReport KEY_MEDIA_WWW_SEARCH = {0, 8};
const MediaKeyReport KEY_MEDIA_WWW_STOP = {0, 16};
const MediaKeyReport KEY_MEDIA_WWW_BACK = {0, 32};
const MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION = {0, 64};
const MediaKeyReport KEY_MEDIA_EMAIL_READER = {0, 128};

typedef struct {
  uint8_t modifier;
  uint8_t reserved;
  uint8_t keycode[6];
} KeyReport;

class BleKeyboard : public Print,
                    public NimBLEServerCallbacks,
                    public NimBLECharacteristicCallbacks {
private:
  NimBLEHIDDevice *_hid;
  NimBLEServer *_server;
  NimBLECharacteristic *_inputKeyboard;
  NimBLECharacteristic *_outputKeyboard;
  NimBLECharacteristic *_inputMediaKeys;
  NimBLEAdvertising *_advertising;
  KeyReport _keyReport;
  MediaKeyReport _mediaKeyReport;
  std::string _deviceName;
  std::string _deviceManufacturer;
  uint8_t _batteryLevel;
  bool _isConnected = false;
  bool _isKeyboardSubscribed = false;
  bool _isMediaKeysSubscribed = false;
  uint8_t _keyboardLedState = 0xFF;
  uint32_t _delay_ms = 20;
  uint16_t _vid = 0x05ac;
  uint16_t _pid = 0x820a;
  uint16_t _version = 0x0210;

public:
  BleKeyboard(std::string deviceName = "ESP32 BLE Keyboard",
              std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
  void begin(void);
  void end(void) { log_i("END"); };

  void sendKeyReport(KeyReport *keys);
  void sendKeyReport(MediaKeyReport *keys);

  void pressKey(uint8_t keyPress, uint8_t modifiers);
  void pressKey(const MediaKeyReport k);

  void releaseKey(uint8_t keyPress);
  void releaseKey(const MediaKeyReport k);
  void releaseAllKeys(void);

  void typeKey(uint8_t keyPress, uint8_t modifiers);
  void typeKey(const MediaKeyReport k);

  size_t write(uint8_t chr);
  size_t write(const MediaKeyReport c);
  size_t write(const uint8_t *buffer, size_t size);

  bool isConnected(void) { return _isConnected; };
  bool isKeyboardSubscribed(void) { return _isKeyboardSubscribed; };
  bool isMediaKeysSubscribed(void) { return _isMediaKeysSubscribed; };
  void setBatteryLevel(uint8_t level);
  void setName(std::string deviceName) { _deviceName = deviceName; };
  void setDelay(uint32_t ms) { _delay_ms = ms; };
  uint8_t getLedState(void) { return _keyboardLedState; };
  void setVendorId(uint16_t vid) { _vid = vid; };
  void setProductId(uint16_t pid) { _pid = pid; };
  void setVersion(uint16_t version) { _version = version; };

protected:
  virtual void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo,
                         std::string &name) override;

  virtual void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override;

  virtual void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override;

  virtual void onWrite(NimBLECharacteristic *me, NimBLEConnInfo &connInfo) override;

  virtual void onSubscribe(NimBLECharacteristic *me, NimBLEConnInfo &connInfo,
                           uint16_t subValue) override;
};

#endif