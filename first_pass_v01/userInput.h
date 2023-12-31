#ifndef H_USER_INPUT
#define H_USER_INPUT

namespace userInput {
  struct TimeChange {
    int8_t hourDiff,
           minuteDiff;
    enum class HourMode {
      NO_CHANGE, TO_TWELVE, TO_TWENTY_FOUR
    } hourMode;
    bool is_changed() const;
  };
  
  bool setup(); // Returns current switch state
  bool has_timed_out();
  TimeChange check_state();
}

#endif