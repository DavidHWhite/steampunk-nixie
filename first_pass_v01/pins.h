#ifndef H_PINS
#define H_PINS

namespace pins {
  namespace cathode {
    constexpr int DATA    = 2,
                  SCLK    = 3,
                  LATCH   = 4,
                  DISABLE = 5;
  }
  
  namespace anode {
    constexpr int DATA    = 6,
                  SCLK    = 7,
                  LATCH   = 8,
                  DISABLE = 9;
  }
  
  namespace rtc {
    constexpr int SDA  = 18, // Unsure if these first two need to be specified since they're the only pins available for I2C
                  SCL  = 19,
                  SQW  = 20,
                  O32K = 21; // Not currently planning to use this, but connecting just in case
  }
  
  namespace input {
    constexpr int HOUR_INC = 10,
                  HOUR_DEC = 11,
                  MIN_INC  = 12,
                  MIN_DEC  = 14, // Skipping 13 so I don't have to work around the built-in LED
                  HOUR_FORMAT_SWITCH = 15;
  }
}

#endif