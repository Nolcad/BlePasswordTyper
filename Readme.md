# BlePasswordTyper

## Overview

**BlePasswordTyper** is an ESP32‑based experimental project that emulates a **Bluetooth Low Energy (BLE) keyboard** to automatically type predefined credentials on a paired host (PC, smartphone, tablet, etc.).

Beyond simple key injection, the project demonstrates **bidirectional communication over standard HID keyboard LEDs**, enabling the host to send information back to the device without custom drivers or BLE services.

⚠️ **Security Warning**  
This project is intentionally insecure and intended for experimentation only. Credentials are stored in firmware and can be trivially recovered.

---

## Architecture Overview

The project is structured into three clearly separated components:

### 1. Application Layer (`src/`)

- Device boot and initialization
- Button handling and user interaction
- Orchestration of credential typing
- Coordination between BLE keyboard and LED signaling

### 2. BleKeyboard Library (`lib/BleKeyboard/`)

- BLE HID keyboard implementation for ESP32
- Sends key press and release events
- Exposes host keyboard LED states (Caps Lock, Num Lock, Scroll Lock)

See `lib/BleKeyboard/README.md` for details.

### 3. KeyboardLedSignaling Library (`lib/KeyboardLedSignaling/`)

- Decodes a **clocked, bit‑level protocol** transmitted via keyboard LED states
- Provides a deterministic state machine and payload API

See `lib/KeyboardLedSignaling/README.md` for details.

---

## Keyboard LED Signaling

The project implements **host‑to‑device communication** using standard HID keyboard LEDs:

- One LED acts as a **clock**
- One LED acts as a **data line**
- Bits are transmitted LSB‑first
- Frame format:
  - Byte 0: payload length
  - Bytes 1..N: payload data

This technique works over standard BLE HID and requires **no custom host drivers**.

---

## Hardware

- **M5StickC** (ESP32‑based)
- No external components required
- Communication relies entirely on BLE HID reports

---

## Build & Flash

### Prerequisites

- Visual Studio Code
- PlatformIO

### Steps

```sh
pio project init --ide=vscode
pio run --target upload
```

---

## Configuration

Refer to `src/main.cpp` for:

- Credential definition
- Button mapping
- BLE keyboard initialization
- Integration with `KeyboardLedSignaling`

---

## Intended Use

This project is intended as:

- A BLE HID learning platform
- A proof‑of‑concept for LED‑based side‑channel communication
- A reusable reference for embedded HID experimentation

It is **not** intended for real‑world secure credential handling.

---

## Shout‑outs

- DigisparkKeyboard – USB keyboard LED handling
- Haunted Doll – LED signaling inspiration
