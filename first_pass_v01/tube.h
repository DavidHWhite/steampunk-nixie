#ifndef H_TUBE
#define H_TUBE

// Bits will physically travel from right to left along the bottom edge of the PCB
// IE the first values sent will end up at the hour10s

namespace tube {
  enum class Tube : int {
    hour10s   = 0,
    hour1s    = 1,
    minute10s = 2,
    minute1s  = 3
  };
  
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
  void set_digit(Tube, DisplayVal);
  void set_time_display(DisplayVal, DisplayVal, DisplayVal, DisplayVal);
  void set_time(int, int, bool);
  void update_display();
  DisplayVal get_displayVal(int);
}

#endif