#include "BleKeyboard.h"

#include <NimBLEDevice.h>

#include "HidDescriptor.h"
#include "KeyLayouts.h"

BleKeyboard::BleKeyboard(std::string deviceName, std::string deviceManufacturer,
                         uint8_t batteryLevel)
    : _hid(0), _deviceName(std::string(deviceName).substr(0, 15)),
      _deviceManufacturer(std::string(deviceManufacturer).substr(0, 15)),
      _batteryLevel(batteryLevel) {}

void BleKeyboard::begin(void) {
  NimBLEDevice::init(_deviceName);
  _server = NimBLEDevice::createServer();
  _server->setCallbacks(this);

  _hid = new NimBLEHIDDevice(_server);
  _inputKeyboard = _hid->inputReport(KEYBOARD_ID);
  _outputKeyboard = _hid->outputReport(KEYBOARD_ID);
  _inputMediaKeys = _hid->inputReport(MEDIA_KEYS_ID);

  _outputKeyboard->setCallbacks(this);
  _inputKeyboard->setCallbacks(this);
  _inputMediaKeys->setCallbacks(this);

  _hid->manufacturer()->setValue(_deviceManufacturer);

  _hid->pnp(0x02, _vid, _pid, _version);
  _hid->hidInfo(0x00, 0x01);

  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
  NimBLEDevice::setSecurityAuth(true, true, true);

  _hid->reportMap((uint8_t *)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  _hid->startServices();

  _advertising = _server->getAdvertising();
  _advertising->setAppearance(HID_KEYBOARD);
  _advertising->addServiceUUID(_hid->hidService()->getUUID());
  _advertising->setScanResponse(false);
  _advertising->start();
  _hid->setBatteryLevel(_batteryLevel);

  log_i("BLE advertising started");
}

void BleKeyboard::setBatteryLevel(uint8_t level) {
  _batteryLevel = level;
  if (_hid != 0)
    _hid->setBatteryLevel(_batteryLevel);
}

void BleKeyboard::sendKeyReport(KeyReport *keys) {
  if (isConnected()) {
    _inputKeyboard->setValue((uint8_t *)keys, sizeof(KeyReport));
    _inputKeyboard->notify();
    delay(_delay_ms);
  }
}

void BleKeyboard::sendKeyReport(MediaKeyReport *keys) {
  if (isConnected()) {
    _inputMediaKeys->setValue((uint8_t *)keys, sizeof(MediaKeyReport));
    _inputMediaKeys->notify();
    delay(_delay_ms);
  }
}

void BleKeyboard::pressKey(uint8_t key, uint8_t modifiers) {
  memset(&_keyReport, 0, sizeof(_keyReport));

  _keyReport.modifier = modifiers;
  _keyReport.keycode[0] = key;
  sendKeyReport(&_keyReport);
}

void BleKeyboard::pressKey(const MediaKeyReport key) {
  uint16_t k_16 = key[1] | (key[0] << 8);
  uint16_t mediaKeyReport_16 = _mediaKeyReport[1] | (_mediaKeyReport[0] << 8);

  mediaKeyReport_16 |= k_16;
  _mediaKeyReport[0] = (uint8_t)((mediaKeyReport_16 & 0xFF00) >> 8);
  _mediaKeyReport[1] = (uint8_t)(mediaKeyReport_16 & 0x00FF);

  sendKeyReport(&_mediaKeyReport);
}

void BleKeyboard::releaseKey(uint8_t key) {
  _keyReport.modifier = 0;
  _keyReport.keycode[0] = 0;

  sendKeyReport(&_keyReport);
}

void BleKeyboard::releaseKey(const MediaKeyReport key) {
  _mediaKeyReport[0] = 0;
  _mediaKeyReport[1] = 0;

  sendKeyReport(&_mediaKeyReport);
}

void BleKeyboard::releaseAllKeys(void) {
  _keyReport.keycode[0] = 0;
  _keyReport.keycode[1] = 0;
  _keyReport.keycode[2] = 0;
  _keyReport.keycode[3] = 0;
  _keyReport.keycode[4] = 0;
  _keyReport.keycode[5] = 0;
  _keyReport.modifier = 0;
  _mediaKeyReport[0] = 0;
  _mediaKeyReport[1] = 0;
  sendKeyReport(&_keyReport);
  sendKeyReport(&_mediaKeyReport);
}

void BleKeyboard::typeKey(uint8_t key, uint8_t modifiers) {
  pressKey(key, modifiers);
  releaseKey(key);
}

void BleKeyboard::typeKey(const MediaKeyReport key) {
  pressKey(key);
  releaseKey(key);
}
uint8_t keycode_to_modifier(KEYCODE_TYPE keycode) {
  uint8_t modifier = 0;

#ifdef SHIFT_MASK
  if (keycode & SHIFT_MASK)
    modifier |= MODIFIERKEY_SHIFT;
#endif
#ifdef ALTGR_MASK
  if (keycode & ALTGR_MASK)
    modifier |= MODIFIERKEY_RIGHT_ALT;
#endif
#ifdef RCTRL_MASK
  if (keycode & RCTRL_MASK)
    modifier |= MODIFIERKEY_RIGHT_CTRL;
#endif
  return modifier;
}

uint8_t keycode_to_key(KEYCODE_TYPE keycode) {
  uint8_t key = keycode;
  // the only valid ASCII code > 63
  if (keycode != KEY_NON_US_BS) {
    key = key & 0x3F;
  }
  return key;
}

size_t BleKeyboard::write(uint8_t chr) {
  uint8_t data = 0;
  if (chr == 0x09) {
    data = (uint8_t)KEY_TAB;
  } else if (chr == 0x0a) {
    data = (uint8_t)KEY_ENTER;
  } else if (chr >= 0x20) {
    data = pgm_read_byte_near(keycodes_ascii + (chr - 0x20));
  }
  if (data) {
    pressKey(keycode_to_key(data), keycode_to_modifier(data));
    releaseKey(keycode_to_key(data));
  }
  return 1;
}

size_t BleKeyboard::write(const MediaKeyReport c) {
  pressKey(c);
  releaseKey(c);
  return 1;
}

size_t BleKeyboard::write(const uint8_t *buffer, size_t size) {
  size_t n = 0;
  while (size--) {
    if (*buffer != '\r') {
      if (write(*buffer)) {
        n++;
      } else {
        break;
      }
    }
    buffer++;
  }
  return n;
}

void BleKeyboard::onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, std::string &name) {
  _isConnected = true;
  log_i("Keyboard is connected - %s", name.c_str());
}

void BleKeyboard::onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) {
  std::string noName = "NoName";
  onConnect(pServer, connInfo, noName);
}

void BleKeyboard::onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) {
  _isConnected = false;
  _isKeyboardSubscribed = false;
  _isMediaKeysSubscribed = false;
  _keyboardLedState = 0xFF;
  log_i("Keyboard is disconnected");
}

void BleKeyboard::onWrite(NimBLECharacteristic *me, NimBLEConnInfo &connInfo) {
  uint8_t value = *(me->getValue().data());
  _keyboardLedState = value;
  log_i("Received keyboard LED state: 0x%02X", value);
}

void BleKeyboard::onSubscribe(NimBLECharacteristic *me, NimBLEConnInfo &connInfo,
                              uint16_t subValue) {
  uint8_t value = *(me->getDescriptorByUUID((uint16_t)0x2908)->getValue().data());
  NimBLEUUID serviceUUID = me->getService()->getUUID();
  NimBLEUUID characteristicUUID = me->getUUID();

  log_d("Service: %s, Characteristic: %s, Value: %d, SubValue: %d", serviceUUID.toString().c_str(),
        characteristicUUID.toString().c_str(), value, subValue);

  bool isSubscribed = (subValue == 1);

  if (serviceUUID == NimBLEUUID((uint16_t)0x1812) &&
      characteristicUUID == NimBLEUUID((uint16_t)0x2a4d)) {
    switch (value) {
    case KEYBOARD_ID:
      _isKeyboardSubscribed = isSubscribed;
      log_i("Keyboard has been %s", isSubscribed ? "subscribed" : "unsubscribed");
      break;
    case MEDIA_KEYS_ID:
      _isMediaKeysSubscribed = isSubscribed;
      log_i("Media Keys have been %s", isSubscribed ? "subscribed" : "unsubscribed");
      break;
    }
  }
}
