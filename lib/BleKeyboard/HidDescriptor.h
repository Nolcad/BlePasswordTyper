#ifndef HIDDESCRIPTOR_H__
#define HIDDESCRIPTOR_H__

#include "HIDTypes.h"

// HID descriptor
#define KEYBOARD_ID   0x01
#define MEDIA_KEYS_ID 0x02

static const uint8_t _hidReportDescriptor[] = {
  USAGE_PAGE(1),      0x01,          	// Usage Page (Generic Desktop Ctrls)
  USAGE(1),           0x06,          	// Usage (Keyboard)
  COLLECTION(1),      0x01,          	// Collection (Application)
  // -------------------------------  Keyboard
  REPORT_ID(1),       KEYBOARD_ID,   	//   Report ID (1)
  USAGE_PAGE(1),      0x07,          	//   Usage Page (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0xE0,          	//   Usage Minimum (0xE0)
  USAGE_MAXIMUM(1),   0xE7,          	//   Usage Maximum (0xE7)
  LOGICAL_MINIMUM(1), 0x00,          	//   Logical Minimum (0)
  LOGICAL_MAXIMUM(1), 0x01,          	//   Logical Maximum (1)
  REPORT_SIZE(1),     0x01,          	//   Report Size (1)
  REPORT_COUNT(1),    0x08,          	//   Report Count (8)
  HIDINPUT(1),        0x02,          	//   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x01,          	//   Report Count (1)
  REPORT_SIZE(1),     0x08,          	//   Report Size (8)
  HIDINPUT(1),        0x01,          	//   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  REPORT_COUNT(1),    0x05,          	//   Report Count (5)
  REPORT_SIZE(1),     0x01,          	//   Report Size (1)
  USAGE_PAGE(1),      0x08,          	//   Usage Page (LEDs)
  USAGE_MINIMUM(1),   0x01,          	//   Usage Minimum (Num Lock)
  USAGE_MAXIMUM(1),   0x05,          	//   Usage Maximum (Kana)
  HIDOUTPUT(1),       0x02,          	//   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x01,          	//   Report Count (1)
  REPORT_SIZE(1),     0x03,          	//   Report Size (3)
  HIDOUTPUT(1),       0x01,          	//   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  REPORT_COUNT(1),    0x06,          	//   Report Count (6)
  REPORT_SIZE(1),     0x08,          	//   Report Size (8)
  LOGICAL_MINIMUM(1), 0x00,          	//   Logical Minimum (0)
  LOGICAL_MAXIMUM(1), 0x65,          	//   Logical Maximum (101)
  USAGE_PAGE(1),      0x07,          	//   Usage Page (Kbrd/Keypad)
  USAGE_MINIMUM(1),   0x00,          	//   Usage Minimum (0x00)
  USAGE_MAXIMUM(1),   0x65,          	//   Usage Maximum (0x65)
  HIDINPUT(1),        0x00,          	//   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0),                 	// End Collection
  // -------------------------------  Media Keys
  USAGE_PAGE(1),      0x0C,          	// Usage Page (Consumer)
  USAGE(1),           0x01,          	// Usage (Consumer Control)
  COLLECTION(1),      0x01,          	// Collection (Application)
  REPORT_ID(1),       MEDIA_KEYS_ID, 	//   Report ID (2)
  USAGE_PAGE(1),      0x0C,          	//   Usage Page (Consumer)
  LOGICAL_MINIMUM(1), 0x00,          	//   Logical Minimum (0)
  LOGICAL_MAXIMUM(1), 0x01,          	//   Logical Maximum (1)
  REPORT_SIZE(1),     0x01,          	//   Report Size (1)
  REPORT_COUNT(1),    0x10,          	//   Report Count (16)
  USAGE(1),           0xB5,          	//   Usage (Scan Next Track)
  USAGE(1),           0xB6,          	//   Usage (Scan Previous Track)
  USAGE(1),           0xB7,          	//   Usage (Stop)
  USAGE(1),           0xCD,          	//   Usage (Play/Pause)
  USAGE(1),           0xE2,          	//   Usage (Mute)
  USAGE(1),           0xE9,          	//   Usage (Volume Increment)
  USAGE(1),           0xEA,          	//   Usage (Volume Decrement)
  USAGE(2),           0x23, 0x02,    	//   Usage (AC Home)
  USAGE(2),           0x94, 0x01,    	//   Usage (AL Local Machine Browser)
  USAGE(2),           0x92, 0x01,    	//   Usage (AL Calculator)
  USAGE(2),           0x2A, 0x02,    	//   Usage (AC Bookmarks)
  USAGE(2),           0x21, 0x02,    	//   Usage (AC Search)
  USAGE(2),           0x26, 0x02,    	//   Usage (AC Stop)
  USAGE(2),           0x24, 0x02,    	//   Usage (AC Back)
  USAGE(2),           0x83, 0x01,    	//   Usage (AL Consumer Control Configuration)
  USAGE(2),           0x8A, 0x01,    	//   Usage (AL Email Reader)
  HIDINPUT(1),        0x02,          	//   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  END_COLLECTION(0)                  	// End Collection
};

#endif