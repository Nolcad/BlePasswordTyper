# App

## Overview

The **App** folder contains **host‑side helper scripts** that interact with the
**BlePasswordTyper** device. These applications demonstrate how a host computer can
**actuate the `KeyboardLedSignaling` protocol** using only standard HID keyboard LED control.

Unlike typical host/device communication channels, these tools require **no custom drivers,
no vendor‑specific BLE services, and no elevated privileges** beyond normal HID access.

---

## Included Scripts

### PowerShell LED Signaling Script

This folder already contains a **PowerShell (`.ps1`) script** that implements the
host‑side of the Keyboard LED signaling protocol.

The script:

- Uses standard Windows APIs to control keyboard LEDs
- Toggles LEDs in a deterministic timing pattern
- Encodes data using a **clock + data LED scheme**
- Sends payload frames compatible with the `KeyboardLedSignaling` decoder

No additional files are required beyond the script itself.

---

## How the Script Works

At a high level, the script performs the following steps:

1. Reads current keyboard LED states
2. Assigns one LED as a **clock** and one as **data**
3. Drives LED transitions using simulated key events
4. Sends bits **LSB‑first**, sampled on clock transitions
5. Frames messages as:
   - First byte: payload length
   - Following bytes: payload data

On the device side, this data is decoded by the `KeyboardLedSignaling` library.

---

## Usage Notes

- The ESP32 device must already be paired and connected as a **BLE keyboard**
- LED changes must be reflected by the operating system (default behavior on Windows)
- Timing delays in the script may need adjustment depending on:
  - Host performance
  - Operating system latency
  - BLE stack behavior

---

## Customization

Typical modifications include:

- Changing which LEDs are used for clock and data
- Adjusting inter‑bit timing delays
- Sending different payloads or command frames
- Wrapping the logic in higher‑level automation scripts

---

## Cross‑Platform Considerations

The provided script targets **Windows / PowerShell**.

Equivalent implementations are possible on other platforms using:

- Python (via system keyboard APIs)
- Native OS automation tools
- USB HID interfaces

---

## Security Notice

These scripts intentionally use a **non‑secure side channel** based on keyboard LED states.

They are provided strictly for **experimentation, debugging, and research** and must not be
used for secure communication or sensitive data transfer.
