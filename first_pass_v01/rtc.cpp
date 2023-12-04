// DS3231 - Version: Latest 
#include <DS3231.h>

#include "rtc.h"
#include "pins.h"

// TODO in setup(), implement this safeguard to stop Alarm 2 from misbehaving:
// https://github.com/NorthernWidget/DS3231/blob/master/Documentation/Alarms.md#how-and-why-to-prevent-an-alarm-entirely

// TODO set up the INT pin for alarms every minute!!!
// https://github.com/NorthernWidget/DS3231/blob/master/Documentation/Alarms.md
// 0b00001110 in alarmBits will setup Alarm 1 to trigger every minute (must set seconds to 0)

namespace rtc {
  static bool hasMinutePassed = true;

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
    // TODO set pin modes
    attachInterrupt(digitalPinToInterrupt(pins::rtc::INT), rtc_isr, RISING);

    // TODO implement
  }

  void apply_time_delta(int hour, int minute) {
    // TODO implement
  }

  void set_hour_mode(HourMode mode) {
    // TODO implement
  }

  void get_time(int* hour, int* minute, bool* isTwentyFourHour) {
    // TODO implement
  }

  bool has_minute_passed() {
    return hasMinutePassed;
  }

  void reset_minute_passed() {
    hasMinutePassed = false;
  }
}