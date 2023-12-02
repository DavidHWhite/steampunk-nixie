// DS3231 - Version: Latest 
#include <DS3231.h>

#include "rtc.h"

// TODO in setup(), implement this safeguard to stop Alarm 2 from misbehaving:
// https://github.com/NorthernWidget/DS3231/blob/master/Documentation/Alarms.md#how-and-why-to-prevent-an-alarm-entirely

// TODO set up the INT pin for alarms every minute!!!
// https://github.com/NorthernWidget/DS3231/blob/master/Documentation/Alarms.md
// 0b00001110 in alarmBits will setup Alarm 1 to trigger every minute (must set seconds to 0)

namespace rtc {
  void setup() {
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
}