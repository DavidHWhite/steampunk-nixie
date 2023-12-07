#ifndef H_RTC
#define H_RTC

namespace rtc {
  enum class HourMode {
    TWELVE, TWENTY_FOUR
  };

  void setup();
  void apply_time_delta(byte hour, byte minute); // Resets seconds if minute != 0
  void set_hour_mode(HourMode mode);
  void get_time(byte* hour, byte* minute, bool* isTwentyFourHour);
  bool has_minute_passed();
  void reset_minute_passed();
}

#endif