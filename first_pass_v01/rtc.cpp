#include <RTClib.h>

#include "rtc.h"
#include "pins.h"
#include "debug_toggle.h"

namespace rtc {
  static RTC_DS3231 rtcModule;
  static volatile bool hasMinutePassed = true;
  static HourMode hourMode = HourMode::TWELVE; // TODO set this up properly

  /*
   * ISR to allow the buttons and switch to wake the MCU from sleep
   */

  void rtc_isr() {
    hasMinutePassed = true;
  }

  /*
   * Publicly accessible RTC operations declared in header
   */

  void setup() {
    pinMode(pins::rtc::INT, INPUT_PULLUP);
    pinMode(pins::rtc::O32K, INPUT_PULLUP);

	  // Initializing the rtc
    // rtcModule.begin(); // TODO switch to just this
    if(!rtcModule.begin()) {
        Serial.println("Couldn't find RTC!");
        Serial.flush();
        while (1) delay(10);
    }
    if(rtcModule.lostPower()) { // TODO play with this
        rtcModule.adjust(DateTime(2001, 1, 1, 12, 0, 0));
    }

    // Not using the 32K Pin, so disable it
    rtcModule.disable32K();

    // Set up an interrupt every minute from the SQW/INT pin
    attachInterrupt(digitalPinToInterrupt(pins::rtc::INT), rtc_isr, FALLING);

    // Clear both alarms and disable alarm 1 to avoid any problems
    rtcModule.clearAlarm(1);
    rtcModule.clearAlarm(2);
    rtcModule.disableAlarm(1);

    // Put SQW/INT pin in the correct mode
    rtcModule.writeSqwPinMode(DS3231_OFF);

    // Set alarm 2 to the correct mode
    rtcModule.setAlarm2(rtcModule.now(), DS3231_A2_PerMinute);
  }

  void apply_time_delta(int8_t hourDelta, int8_t minuteDelta) {
    DateTime oldTime = rtcModule.now();
    uint8_t newHour = (24 + oldTime.hour() + hourDelta) % 24;
    uint8_t newMinute = (60 + oldTime.minute() + minuteDelta) % 60;
    uint8_t newSecond = minuteDelta != 0 ? 0 : oldTime.second();
    rtcModule.adjust(DateTime(2001, 1, 1, newHour, newMinute, newSecond));
  }

  void set_hour_mode(HourMode newMode) {
    hourMode = newMode;
  }

  bool get_time(int8_t* o_hour, int8_t* o_minute, HourMode* o_hourMode) {
    DateTime time = rtcModule.now();
    *o_hour = hourMode == HourMode::TWELVE
                ? time.twelveHour()
                : time.hour();
    *o_minute = time.minute();
    *o_hourMode = hourMode;
    return time.isPM();
  }

  bool has_minute_passed() {
    return hasMinutePassed;
  }

  void reset_minute_passed() {
    hasMinutePassed = false;
    rtcModule.clearAlarm(1);
    rtcModule.clearAlarm(2);
  }
}