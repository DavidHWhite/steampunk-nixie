#include "Arduino.h"

#include "display.h"
#include "pins.h"
#include "debug_toggle.h"

namespace display {

  /*
   * Hidden forward declarations
   */
  
  namespace anode {
    static void setup_pins();
    static void setup_run_ops();
    static void write_from_values(DisplayVal, DisplayVal, DisplayVal, DisplayVal);
    static void update_display();
  }
  
  namespace cathode {
    static void setup_pins();
    static void setup_run_ops();
    static void write_from_values(DisplayVal, DisplayVal, DisplayVal, DisplayVal);
    static void update_display();
  }
  
  /*
   * Publicly accessible tube operations declared in header
   */
  
  void setup() {
    // Set correct pin modes
    cathode::setup_pins();
    anode::setup_pins();

    // Disable cathode reset (should ensure the registers start at 0x00)
    cathode::setup_run_ops();
    anode::setup_run_ops();
    
    // Write 'zero' to all tubes
    set_display_digits(DisplayVal::zero, DisplayVal::zero, DisplayVal::zero, DisplayVal::zero);
  }

  void set_display_digits(DisplayVal hour10, DisplayVal hour1, DisplayVal min10, DisplayVal min1) {
    cathode::write_from_values(hour10, hour1, min10, min1);
    anode::write_from_values(hour10, hour1, min10, min1);
    cathode::update_display();
    anode::update_display();
  }

  void set_time_display(byte hour, byte minute, bool isTwentyFourHour) {
    set_display_digits(
      hour >= 10 || isTwentyFourHour
        ? static_cast<DisplayVal>(hour / 10)
        : DisplayVal::none,
      static_cast<DisplayVal>(hour % 10),
      static_cast<DisplayVal>(minute / 10),
      static_cast<DisplayVal>(minute % 10)
    );
  }
  
  DisplayVal get_displayVal(int val) {
    return static_cast<DisplayVal>(val);
  }
  
  /*
   * Anode implementation
   */
  
  namespace anode {
    static void setup_pins() {
    }

    static void setup_run_ops() {
    }

    static void write_from_values(DisplayVal h10, DisplayVal h1, DisplayVal m10, DisplayVal m1) {
    }

    static void update_display() {
    }
  }
  
  /*
   * Cathode implementation
   */

  namespace cathode {
    static void setup_pins() {
      pinMode(pins::cathode::DATA, OUTPUT);
      pinMode(pins::cathode::SCLK, OUTPUT);
      pinMode(pins::cathode::LATCH, OUTPUT);
      pinMode(pins::cathode::RESET_INV, OUTPUT);
    }

    static void setup_run_ops() {
      digitalWrite(pins::cathode::RESET_INV, HIGH);
    }

    static void write_from_values(DisplayVal h10, DisplayVal h1, DisplayVal m10, DisplayVal m1) {
      DisplayVal vals[4] = {h10, h1, m10, m1};
      uint64_t valToSend = 0; // Only lower 40 bits will be used
      for (int i = 0; i < 4; ++i) {
        valToSend <<= 10;
        if (vals[i] != DisplayVal::none) {
          valToSend |= 0b1 << static_cast<int>(vals[i]);
        }
      }
#if DEBUG
      Serial.println(1099511627776 + valToSend, BIN);
      Serial.println(F(" '`'`'`'`'`.,.,.,.,.,'`'`'`'`'`.,.,.,.,.,"));
#endif
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, valToSend & 0xFF);
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, (valToSend >> 8) & 0xFF);
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, (valToSend >> 16) & 0xFF);
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, (valToSend >> 24) & 0xFF);
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, (valToSend >> 32) & 0xFF);
    }

    static void update_display() {
      digitalWrite(pins::cathode::LATCH, HIGH);
      digitalWrite(pins::cathode::LATCH, LOW);
    }
  }
}