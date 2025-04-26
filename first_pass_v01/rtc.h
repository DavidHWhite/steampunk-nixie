#ifndef H_RTC
#define H_RTC

namespace rtc {
  enum class HourMode {
    TWELVE, TWENTY_FOUR
  };

  enum class Granularity {
      MINUTES, SECONDS
  };

  void setup(Granularity granularity);
  void apply_time_delta(int8_t hour, int8_t minute); // Resets seconds if minute != 0
  void set_hour_mode(HourMode mode);
  bool get_time(int8_t* hour, int8_t* minute, int8_t* o_second, HourMode* hourMode); // Returns true for PM
  bool has_time_passed();
  void reset_time_passed();
}

#endif