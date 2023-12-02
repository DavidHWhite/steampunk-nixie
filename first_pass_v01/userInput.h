#ifndef H_USER_INPUT
#define H_USER_INPUT

namespace userInput {
  struct TimeChange {
    int hourDiff,
        minuteDiff;
    enum class HourMode {
      NO_CHANGE, TO_TWELVE, TO_TWENTY_FOUR
    } hourMode;

    bool is_changed();
  };
  
  void setup();
  bool has_timed_out();
  TimeChange check_state();
}

#endif