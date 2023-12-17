#ifndef H_DISPLAY
#define H_DISPLAY

namespace display {
  enum class DisplayVal : int {
    zero  = 0,
    one   = 1,
    two   = 2,
    three = 3,
    four  = 4,
    five  = 5,
    six   = 6,
    seven = 7,
    eight = 8,
    nine  = 9,
    none
  };
  
  void setup();
  void set_display_digits(DisplayVal, DisplayVal, DisplayVal, DisplayVal);
  void set_time_display(byte, byte, bool isTwentyFourHour);
  DisplayVal get_displayVal(int);
}

#endif