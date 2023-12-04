#ifndef H_PINS
#define H_PINS

namespace pins {
  namespace cathode {
    constexpr int DATA           =  2,
                  SCLK           =  3,
                  LATCH          =  6, // TODO add a pulldown resistor here!!!
                  RESET_INV      =  7;
  }
  
  namespace anode {
    constexpr int DATA           =  8,
                  SCLK           =  9,
                  LATCH          = 10,
                  RESET_INV      = 11, // TODO add a pulldown resistor here!!!4
                  OUTPUT_DISABLE = 21;
  }
  
  namespace rtc {
    // Not currently planning to use O32K but I'm connecting it just in case
    constexpr int SDA  = 18, // Unsure if these first two need to be specified since they're the only pins available for I2C
                  SCL  = 19,
                  INT  = 15, // Full async detection for interrupts!
                  O32K = 20;
  }
  
  namespace input {
    /*
      IMPORTANT NOTE ABOUT INTERRUPTS WHILE THE CHIP IS ASLEEP:
      Only a few pins support full asynchronous interrupt detection, see I/O Multiplexing and Considerations.
      These limitations apply for waking the system from sleep:
        tl;dr only the fully async pins can wake on a rising or falling edge rather than just "change" or "level"
      Pins Px2 and Px6 of each port have full asynchronous detection.
        This only matches a few pins on the Arduino pinout:
        - D4 (PC6), D5 (PB2), and D15 (PD2) are all fully available (3 pins)
        - D13 (PE2) is attached to the built-in LED but could theoretically also be used
        - D18 (PA2/PF2) is also SDA and thus will be used for the RTC
      
      Since there aren't 5 fully async pins available, I'll put the format switch on an async pin
      and then I'll connect a diode OR of the four buttons to another async pin. That way I don't
      need any interrupts set up for the individual buttons.
        Oh and rtc::INT also needs an async pin.

      Note that these interrupts (with the exception of SQW) are EXCLUSIVELY being used
      to wake the board from sleep. They aren't being used to set any state
    */
    constexpr int HOUR_INC           = 12,
                  HOUR_DEC           = 14,
                  MIN_INC            = 16,
                  MIN_DEC            = 17,
                  BUTTON_OR          =  4, // Full async detection for interrupts!
                  HOUR_FORMAT_SWITCH =  5; // Full async detection for interrupts!
  }
}

// I'm avoiding pin 13 just so I don't have to deal with the built-in LED

#endif