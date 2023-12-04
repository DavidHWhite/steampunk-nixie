#include "Arduino.h"

#include "tube.h"
#include "pins.h"

namespace tube {

  // TODO add screen lines to cathode shift register!!!

  /*
   * Hidden forward declarations
   */
  
  namespace anode {
    enum class Anode : int {
      odd  = 0, // TODO figure out which should be 0 vs 1
      even = 1,
      neither
    };
    void setup();
    void set(Tube, Anode);
    void update_display();
    Anode from_value(DisplayVal);
  }
  
  namespace cathode {
    enum class Cathode : int {
      v01 = 0,
      v23 = 1,
      v45 = 2,
      v67 = 3,
      v89 = 4,
      // none
    };
    void setup();
    void set(Tube, Cathode);
    void update_display();
    Cathode from_value(DisplayVal);
  }
  
  /*
   * Publicly accessible tube operations declared in header
   */
  
  void setup() {
    cathode::setup();
    anode::setup();

    // MUST be in this order!
    // TODO set pins::anode::OUTPUT_DISABLE HIGH
    // TODO enable cathodes RESET_INV
    // TODO enable anodes RESET_INV
    
    // TODO load saved time from RTC instead of this
    set_digit(Tube::hour10s,   DisplayVal::one);
    set_digit(Tube::hour1s,    DisplayVal::two);
    set_digit(Tube::minute10s, DisplayVal::zero);
    set_digit(Tube::minute1s,  DisplayVal::zero);
    
    // TODO add small delay if this isn't turning on properly?
    update_display();

    // TODO set pins::anode::OUTPUT_DISABLE LOW
    // MAKE SURE that good data has been loaded here before doing this!!!
  }
  
  void set_digit(Tube tube, DisplayVal val) {
    cathode::set(tube, cathode::from_value(val)); // This will be v89 for displaying 'none' but it doesn't ultimately matter
    anode::set(tube, anode::from_value(val));
  }
  
  void set_time_display(DisplayVal hour10, DisplayVal hour1, DisplayVal min10, DisplayVal min1) {
    set_digit(Tube::hour10s,   hour10);
    set_digit(Tube::hour1s,    hour1);
    set_digit(Tube::minute10s, min10);
    set_digit(Tube::minute1s,  min1);
  }

  void set_time(int hour, int minute, bool isTwentyFourHour) {
    set_time_display(
      hour >= 10 && isTwentyFourHour
        ? static_cast<DisplayVal>(hour / 10)
        : DisplayVal::none,
      static_cast<DisplayVal>(hour % 10),
      static_cast<DisplayVal>(minute / 10),
      static_cast<DisplayVal>(minute % 10)
    );
  }
  
  void update_display() {
    // The cathode MUST be changed first to ensure that 
    cathode::update_display();
    anode::update_display();
  }
  
  DisplayVal get_displayVal(int val) {
    return static_cast<DisplayVal>(val);
  }
  
  /*
   * Anode implementation
   */
  
  namespace anode {
    static byte state = 0;
    
    void setup() {
      pinMode(pins::anode::DATA, OUTPUT);
      pinMode(pins::anode::SCLK, OUTPUT);
      pinMode(pins::anode::LATCH, OUTPUT);
      pinMode(pins::anode::RESET_INV, OUTPUT);
      pinMode(pins::anode::OUTPUT_DISABLE, OUTPUT);
    }
    
    void set(Tube tube, Anode anode) {
      // Remove any previously enabled anode for this tube
      state &= ~(0b11 << (static_cast<int>(tube) * 2));
      // Write the new anode
      if (anode != Anode::neither) {
        state |= (0b1 << static_cast<int>(anode)) << (static_cast<int>(tube) * 2);
      }
    }
    
    void update_display() {
      // Assumes latch and clock are already low
      shiftOut(pins::anode::DATA, pins::anode::SCLK, LSBFIRST, state); // TODO maybe use MSBFIRST?
      digitalWrite(pins::anode::LATCH, HIGH);
      digitalWrite(pins::anode::LATCH, LOW);
    }
    
    Anode from_value(DisplayVal val) {
      switch (val) {
        case DisplayVal::none:
          return Anode::neither;
        case DisplayVal::zero:
        case DisplayVal::two:
        case DisplayVal::four:
        case DisplayVal::six:
        case DisplayVal::eight:
          return Anode::even;
        default:
          return Anode::odd;
      }
    }
  }
  
  /*
   * Cathode implementation
   */
  
  namespace cathode {
    static uint32_t state = 0;
  
    void setup() {
      pinMode(pins::cathode::DATA, OUTPUT);
      pinMode(pins::cathode::SCLK, OUTPUT);
      pinMode(pins::cathode::LATCH, OUTPUT);
      pinMode(pins::cathode::RESET_INV, OUTPUT);
    }
    
    void set(Tube tube, Cathode cathode) {
      // Remove any previously enabled cathode for this tube
      state &= ~(0b11111 << (static_cast<int>(tube) * 5));
      // Write the new cathode
      state |= (0b1 << static_cast<int>(cathode)) << (static_cast<int>(tube) * 5);
    }
    
    void update_display() {
      // Assumes latch and clock are already low
      uint32_t sending = state;
      for (int i = 0; i < 20; ++i) {
        bool bit = sending & 0b01;
        sending >>= 1;
        
        // Send the bit via serial
        // At 16MHz, each clock cycle is 62.5 ns
        // TODO verify that I don't need to add any delays in here
        digitalWrite(pins::cathode::DATA, bit ? LOW : HIGH);
        digitalWrite(pins::cathode::SCLK, HIGH);
        digitalWrite(pins::cathode::SCLK, LOW);
      }
      digitalWrite(pins::cathode::LATCH, HIGH);
      digitalWrite(pins::cathode::LATCH, LOW);
    }
    
    Cathode from_value(DisplayVal val) {
      switch (val) {
        case DisplayVal::none: // This way one cathode will always be trying to sink current
        case DisplayVal::zero:
        case DisplayVal::one:
          return Cathode::v01;
        case DisplayVal::two:
        case DisplayVal::three:
          return Cathode::v23;
        case DisplayVal::four:
        case DisplayVal::five:
          return Cathode::v45;
        case DisplayVal::six:
        case DisplayVal::seven:
          return Cathode::v67;
        default:
          return Cathode::v89;
      }
    }
  }

}