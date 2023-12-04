#ifndef H_RTC
#define H_RTC

namespace rtc {
  enum class HourMode {
    TWELVE, TWENTY_FOUR
  };

  void setup();
  void apply_time_delta(int hour, int minute); // Resets seconds if minute != 0
  void set_hour_mode(HourMode mode);
  void get_time(int* hour, int* minute, bool* isTwentyFourHour);
  bool has_minute_passed();
  void reset_minute_passed();
}

#endif