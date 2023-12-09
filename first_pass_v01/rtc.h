#ifndef H_RTC
#define H_RTC

namespace rtc {
  enum class HourMode {
    TWELVE, TWENTY_FOUR
  };

  void setup();
  void apply_time_delta(int8_t hour, int8_t minute); // Resets seconds if minute != 0
  void set_hour_mode(HourMode mode);
  bool get_time(int8_t* hour, int8_t* minute, HourMode* hourMode); // Returns true for PM
  bool has_minute_passed();
  void reset_minute_passed();
}

#endif