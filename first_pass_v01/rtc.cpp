#include <RTClib.h>

#include "rtc.h"
#include "pins.h"
#include "debug_toggle.h"

namespace rtc {
  static RTC_DS3231 rtcModule;
  static volatile bool hasTimePassed = true;
  static HourMode hourMode = HourMode::TWELVE; // Will be set to match the switch during the main file's setup()

  /*
   * ISR to allow the buttons and switch to wake the MCU from sleep
   */

  void rtc_isr() {
    hasTimePassed = true;
  }

  /*
   * Publicly accessible RTC operations declared in header
   */

  void setup(Granularity granularity) {
    pinMode(pins::rtc::INT, INPUT_PULLUP);
    pinMode(pins::rtc::O32K, INPUT_PULLUP);

	  // Initializing the rtc
    if(!rtcModule.begin()) {
#if DEBUG
        Serial.println(F("Couldn't find RTC!"));
#endif
        while (1) delay(10);
    }
    if(rtcModule.lostPower()) {
        rtcModule.adjust(DateTime(2001, 1, 1, 12, 0, 0));
    }

    // Not using the 32K Pin, so disable it
    rtcModule.disable32K();

    // Set up an interrupt every minute from the SQW/INT pin
    attachInterrupt(digitalPinToInterrupt(pins::rtc::INT), rtc_isr, FALLING);

    // Clear both alarms and disable alarm 2 to avoid any problems
    rtcModule.clearAlarm(1);
    rtcModule.clearAlarm(2);
    rtcModule.disableAlarm(2);

    // Put SQW/INT pin in the correct mode
    rtcModule.writeSqwPinMode(DS3231_OFF);

    // Set alarm 1 to the correct mode
    rtcModule.setAlarm1(DateTime(0, 0, 0, 0, 0, 0),
                        granularity == Granularity::MINUTES ? DS3231_A1_Second : DS3231_A1_PerSecond);
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

  bool get_time(int8_t* o_hour, int8_t* o_minute, int8_t* o_second, HourMode* o_hourMode) {
    DateTime time = rtcModule.now();
    *o_hour = hourMode == HourMode::TWELVE
                ? time.twelveHour()
                : time.hour();
    *o_minute = time.minute();
    *o_second = time.second();
    *o_hourMode = hourMode;
    return time.isPM();
  }

  bool has_time_passed() {
    return hasTimePassed;
  }

  void reset_time_passed() {
    hasTimePassed = false;
    rtcModule.clearAlarm(1);
    rtcModule.clearAlarm(2);
  }
}