# BlePasswordTyper

## Overview

This project emulates a BLE (Bluetooth Low Energy) keyboard to type a username and password automatically.

Use it as an example if you need to port this to another device.

Except for the screen, power management, HMI and device specific things, the ESP32 code should be generic enough.

Most magic happens in the [BleKeyboard class](lib/BleKeyboard).

Also, this unit served as an experiment platform to the AXP192 PMU (battery charging and gauge management).

> [!WARNING]
> It is definitively not recommended to use this device.
> The username and password can be recovered with a mere text editor by anyone.
> Pretty weak...

---

## Architecture Overview

The project is structured into four clearly separated components:

### 1. Application Layer (`src/`)

- Device boot and initialization
- Button handling and user interaction
- Orchestration of credential typing
- Coordination between BLE keyboard output and LED‑based input signaling

### 2. BleKeyboard Library (`lib/BleKeyboard/`)

- BLE HID keyboard implementation for ESP32
- Sends key press and release events
- Exposes host keyboard LED states (Caps Lock, Num Lock, Scroll Lock)

See `lib/BleKeyboard/README.md` for details.

### 3. KeyboardLedSignaling Library (`lib/KeyboardLedSignaling/`)

- Decodes a **clocked, bit‑level protocol** transmitted via keyboard LED states
- Implements a deterministic state machine and payload API

See `lib/KeyboardLedSignaling/README.md` for details.

### 4. Host Applications (`apps/`)

- Host‑side helper scripts that interact with the device
- Demonstrate how a computer can **actuate the KeyboardLedSignaling protocol**
- Use only standard HID keyboard LED control (no drivers, no custom services)

The folder currently includes a **PowerShell script** that toggles keyboard LEDs in a controlled timing pattern to send framed payloads back to the device.

See `apps/README.md` for details.

---

## Keyboard LED Signaling

The project implements **host‑to‑device communication** using standard HID keyboard LEDs:

- One LED acts as a **clock**
- One LED acts as a **data line**
- Bits are transmitted **LSB‑first**
- Frame format:
  - Byte 0: payload length
  - Bytes 1..N: payload data

This technique works over standard BLE HID and requires **no custom host drivers**.

---

## Hardware

- **M5StickC** (ESP32‑based)
- No external components required
- Communication relies entirely on BLE HID reports and LED feedback

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

## Configuration

See [main.cpp](./src/main.cpp) file...

---

## Shout-outs
 - [Danjovic](https://github.com/Danjovic/DigistumpArduino/tree/master/digistump-avr/libraries/DigisparkKeyboard) - USB Keyboard with LED
 - [Axlan](https://github.com/axlan/haunted_doll/tree/main) - USB Keyboard with LED #2