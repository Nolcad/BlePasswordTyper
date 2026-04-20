# KeyboardLedSignaling

## Overview

**KeyboardLedSignaling** is a small, self‑contained library that decodes a **clocked, bit‑level protocol transmitted over standard HID keyboard LED states**.

It allows a device acting as a Bluetooth or USB HID keyboard to **receive data from the host** using only Caps Lock, Num Lock, or Scroll Lock LED output reports—without custom drivers, vendor‑specific extensions, or additional communication channels.

---

## Concept

Standard HID keyboards receive LED state updates from the host. This library repurposes those updates as a signaling mechanism:

- One LED acts as a **clock**
- One LED acts as a **data line**
- Data is sampled on clock transitions

This creates a very low‑bandwidth, host‑to‑device communication channel that remains fully HID‑compliant.

---

## Protocol Description

### Bit Order

- **Least Significant Bit (LSB) first**
- One bit transmitted per clock edge

### Frame Format

```text
+------------+-------------------+
| Byte 0     | Payload size (N)  |
+------------+-------------------+
| Byte 1..N  | Payload bytes     |
+------------+-------------------+
```

- The first byte defines the number of payload bytes to follow
- Reception completes automatically once all bytes are received

---

## State Machine

The decoder operates as an explicit state machine:

- **Idle**  
  Waiting for a start condition (clock activity)

- **Receiving**  
  Clock‑driven bit reception and byte assembly

- **Complete**  
  Full payload received; awaiting user reset

The current state can be queried at any time by the application.

---

## Public API

### Update Loop

```cpp
bool Update(bool clockLed, bool dataLed);
```

Call repeatedly (typically from `loop()`), passing the current LED states.

Returns `true` while a transmission is actively in progress.

---

### State Queries

```cpp
bool isIdling() const;
bool isCommunicating() const;
bool isComplete() const;
```

---

### Payload Access

```cpp
uint8_t getPayloadSize() const;
uint8_t getPayloadCurrentCount() const;
String  getPayloadString() const;
```

---

### Reset

```cpp
bool reset();
```

Resets the decoder to the Idle state after a complete reception.

---

## Typical Usage

```cpp
#include <KeyboardLedSignaling.h>

KeyboardLedSignaling kls;

void loop() {
    kls.Update(clockLed, dataLed);

    if (kls.isComplete()) {
        String message = kls.getPayloadString();
        // Process message
        kls.reset();
    }
}
```

---

## Design Characteristics

- Fully deterministic behavior
- No dynamic memory allocation during decoding
- Independent of BLE / USB / HID transport layers
- Suitable for reuse in other HID‑based projects

---

## Intended Use

- Host‑to‑device configuration channels
- Debug or control signaling
- HID side‑channel experiments
- Research and educational projects

⚠️ **This library is experimental and not intended for robust or secure communication.**
