# BLE Password Typer

This project emulates a BLE (Bluetooth Low Energy) keyboard to type a username and password automatically.

Use it as an example if you need to port this to another device.

Except for the screen, power management, HMI and device specific things, the ESP32 code should be generic enough.

Most magic happens in the [BleKeyboard class](lib/BleKeyboard).

Also, this unit served as an experiment platform to the AXP192 PMU (battery charging and gauge management).

> [!WARNING]
> It is definitively not recommended to use this device.
> The username and password can be recovered with a mere text editor by anyone.
> Pretty weak...

## Usage

1. Power up
2. Search and pair with your host (PC, smartphone...)
3. Push the big button to type the sequence

### Hardware

Runs on an [M5StickC](https://shop.m5stack.com/products/stick-c).

No other hardware is required

## Installation/Programming

### Prerequisites
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO](https://platformio.org/)

### Procedure

1. Download and open the project in VSCode/PlatformIO

1. Initialize the project (terminal within VSCode)
   ```shell
   pio project init --ide=vscode
   ```

1. Build and upload the project

## Configuration

See [main.cpp](./src/main.cpp) file...

## Shout-outs
 - [Danjovic](https://github.com/Danjovic/DigistumpArduino/tree/master/digistump-avr/libraries/DigisparkKeyboard) - USB Keyboard with LED
 - [Axlan](https://github.com/axlan/haunted_doll/tree/main) - USB Keyboard with LED #2