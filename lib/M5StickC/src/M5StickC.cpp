// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "M5StickC.h"

M5StickC::M5StickC() : isInited(0) {
}

void M5StickC::begin(bool LCDEnable, bool PowerEnable, bool SerialEnable) {
    //! Correct init once
    if (isInited)
        return;
    else
        isInited = true;

    //! UART
    if (SerialEnable) {
        Serial.begin(115200);
        Serial.flush();
        delay(50);
    }

    log_i("M5Stick initialization");

    // Power
    if (PowerEnable) {
        log_i(" - AXP192");
        Axp.begin();
    } else {
        log_i(" - AXP192 Disabled");
    }

    // LCD INIT
    if (LCDEnable) {
        log_i(" - LCD");
#ifdef M5STICKC_USE_LEGACY_DISPLAY
        Lcd.begin();
#else
        log_w("LCD is requested but has been disabled at compile time");
#endif
    } else {
        log_i(" - LCD Disabled");
    }

    log_i(" - RTC");
    Rtc.begin();

    log_i("M5StickC initialization complete");
}

void M5StickC::update() {
    M5.BtnA.read();
    M5.BtnB.read();
}

M5StickC M5;