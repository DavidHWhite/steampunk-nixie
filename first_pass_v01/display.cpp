#include "Arduino.h"

#include "display.h"
#include "pins.h"
#include "debug_toggle.h"

namespace display {

  /*
   * Hidden forward declarations
   */
  
  namespace anode {
    static void setup();
    static void write_from_values(DisplayVal, DisplayVal, DisplayVal, DisplayVal);
    static void update_display();
  }
  
  namespace cathode {
    static void setup();
    static void write_from_values(DisplayVal, DisplayVal, DisplayVal, DisplayVal);
    static void update_display();
  }
  
  /*
   * Publicly accessible tube operations declared in header
   */
  
  void setup() {
    cathode::setup();
    anode::setup();

    // Everything here MUST be in this order!
    // This is ensuring that no anodes are fed high voltage while no cathode is yet enabled
    // (This probably doesn't actually matter with the cathode drivers I'm using)

    // disable anode register output
    digitalWrite(pins::anode::OUTPUT_DISABLE, HIGH);
    // disable cathode reset
    digitalWrite(pins::cathode::RESET_INV, HIGH);
    // disable anode reset
    digitalWrite(pins::anode::RESET_INV, HIGH);
    
    // Write 'none' to all tubes
    set_display_digits(DisplayVal::none, DisplayVal::none, DisplayVal::none, DisplayVal::none);

    // enable anode register output
    // MAKE SURE that good data has been loaded here before doing this!!!
    digitalWrite(pins::anode::OUTPUT_DISABLE, LOW);
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
    static void setup() {
      pinMode(pins::anode::DATA, OUTPUT);
      pinMode(pins::anode::SCLK, OUTPUT);
      pinMode(pins::anode::LATCH, OUTPUT);
      pinMode(pins::anode::RESET_INV, OUTPUT);
      pinMode(pins::anode::OUTPUT_DISABLE, OUTPUT);
    }

    static void write_from_values(DisplayVal h10, DisplayVal h1, DisplayVal m10, DisplayVal m1) {
      DisplayVal vals[4] = {h10, h1, m10, m1};
      byte valToSend = 0;
      for (int i = 0; i < 4; ++i) {
        valToSend <<= 2;
        if (vals[i] != DisplayVal::none) {
          valToSend |= static_cast<int>(vals[i]) % 2 == 0
                        ? 0b10
                        : 0b01;
        }
      }
#if DEBUG
      Serial.println(256 + valToSend, BIN);
      Serial.println(F(" ''..''.."));
#endif
      shiftOut(pins::anode::DATA, pins::anode::SCLK, LSBFIRST, valToSend);
      digitalWrite(pins::anode::LATCH, HIGH);
      digitalWrite(pins::anode::LATCH, LOW);
    }

    static void update_display() {
      digitalWrite(pins::anode::LATCH, HIGH);
      digitalWrite(pins::anode::LATCH, LOW);
    }
  }
  
  /*
   * Cathode implementation
   */

  namespace cathode {
    static void setup() {
      pinMode(pins::cathode::DATA, OUTPUT);
      pinMode(pins::cathode::SCLK, OUTPUT);
      pinMode(pins::cathode::LATCH, OUTPUT);
      pinMode(pins::cathode::RESET_INV, OUTPUT);
    }

    static void write_from_values(DisplayVal h10, DisplayVal h1, DisplayVal m10, DisplayVal m1) {
      DisplayVal vals[4] = {h10, h1, m10, m1};
      uint32_t valToSend = 0; // Only lower 24 bits will be used
      for (int i = 0; i < 4; ++i) {
        valToSend <<= 6;
        if (vals[i] != DisplayVal::none) {
          valToSend |= 0b1 << (static_cast<int>(vals[i]) / 2);
        }
      }
#if DEBUG
      Serial.println(16777216 + valToSend, BIN);
      Serial.println(F(" ''''''......''''''......"));
#endif
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, valToSend & 0xFF);
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, (valToSend >> 8) & 0xFF);
      shiftOut(pins::cathode::DATA, pins::cathode::SCLK, LSBFIRST, (valToSend >> 16) & 0xFF);
    }

    static void update_display() {
      digitalWrite(pins::cathode::LATCH, HIGH);
      digitalWrite(pins::cathode::LATCH, LOW);
    }
  }
}