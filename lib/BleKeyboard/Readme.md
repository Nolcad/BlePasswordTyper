# BleKeyboard

## Overview

**BleKeyboard** is a Bluetooth® Low Energy (BLE) **Human Interface Device (HID) keyboard** library for ESP32, aligned with the widely used public Arduino **BleKeyboard** API.

It allows an ESP32 to advertise and operate as a **standard Bluetooth keyboard**, compatible with Windows, Linux, macOS, Android, and iOS hosts.

In addition to sending key events, the library exposes **host keyboard LED output states** (Caps Lock, Num Lock, Scroll Lock), which can be used for feedback mechanisms or higher‑level signaling protocols.

---

## Key Features

- BLE HID keyboard device
- Arduino‑style API
- ASCII and HID keycode support
- Modifier keys (Ctrl, Alt, Shift, GUI)
- Host keyboard LED state reporting:
  - Caps Lock
  - Num Lock
  - Scroll Lock

---

## API Alignment

This library follows the naming and behavior conventions of the public Arduino **BleKeyboard** API:

- `BleKeyboard` class
- `begin()` to start BLE services
- `isConnected()` to check connection state
- `press()`, `release()`, `releaseAll()` for key handling
- `print()` for text output

Developers familiar with the standard BleKeyboard library can use this implementation without relearning the API.

---

## Basic Usage

```cpp
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

void setup() {
    bleKeyboard.begin();
}

void loop() {
    if (!bleKeyboard.isConnected()) {
        return;
    }

    bleKeyboard.print("Hello, world!
");
    delay(1000);
}
```

---

## Key Input API

### Sending Keys

```cpp
bleKeyboard.press('a');
bleKeyboard.release('a');
```

### Modifier Keys

```cpp
bleKeyboard.press(KEY_LEFT_CTRL);
bleKeyboard.press('c');
bleKeyboard.releaseAll();
```

### Text Convenience

```cpp
bleKeyboard.print("Password123");
```

---

## Connection State

```cpp
if (bleKeyboard.isConnected()) {
    // Safe to send key events
}
```

Key reports are only transmitted while a host is connected.

---

## Host LED State Access

According to the HID specification, the host sends **output reports** to indicate keyboard LED states.

BleKeyboard exposes these LED states to the application, enabling:

- Detection of Caps Lock / Num Lock / Scroll Lock changes
- Device‑side status feedback
- Synchronization or acknowledgment signals
- LED‑based side‑channel or signaling protocols

LED states are updated automatically when the host toggles them.

---

## Integration with KeyboardLedSignaling

BleKeyboard is commonly paired with the **KeyboardLedSignaling** library:

- **BleKeyboard**
  - Manages BLE HID transport
  - Sends key reports
  - Receives host LED output reports

- **KeyboardLedSignaling**
  - Interprets LED transitions
  - Implements a clocked bit‑level protocol
  - Reconstructs payload data

---

## Platform Support

- ESP32
- Arduino framework
- PlatformIO or Arduino IDE

---

## Limitations

- Limited bandwidth (HID LED reports)
- Not intended for high‑speed input injection
- No built‑in security beyond standard BLE HID behavior

---

## Intended Use

- BLE HID experimentation
- Embedded tooling and test devices
- HID protocol research
- LED‑based signaling mechanisms

⚠️ **Not suitable for security‑critical or consumer‑grade products.**
