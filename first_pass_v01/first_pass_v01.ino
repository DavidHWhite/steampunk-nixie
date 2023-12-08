#include <avr/sleep.h>  // TODO remove if not using
#include <DS3231.h>

#include "tube.h"
#include "rtc.h"
#include "userInput.h"
#include "pins.h"  // TODO remove

// TODO while testing, indicate sleep with the onboard LED

// TODO disable the userInput interrupts if I'm not using them

void setup() {
  rtc::setup();
  tube::setup();
  bool isTwelveHourMode = userInput::setup();
  rtc::set_hour_mode(isTwelveHourMode ? rtc::HourMode::TWELVE : rtc::HourMode::TWENTY_FOUR);

  // TODO add pulldown resistors to pins::anode::RESET_INV and pins::cathode::RESET_INV


  // TODO Option: instead of enabling/disabling interrupts when entering & exiting sleep:
  // Have all three interrupts always active
  // 1. RTC INT sets displayNeedsUpdate
      // Rising edge only
  // 2. BUTTON_OR just wakes the MCU
      // Both edges
  // 3. SWITCH just wakes the MCU
      // Both edges

  Serial.begin(115200);
  Serial.println("Beginning program...");
}

void loop() {
//   if (userInput::has_timed_out()) {
//     // TODO enter sleep mode
//   }

  // At this point, either a minute has passed or a user input was triggered.
  // (The above is only true if I implement sleep mode)
  // Scan user inputs
  userInput::TimeChange timeChange = userInput::check_state();

  // If something needs to be written to the RTC, do so and set displayNeedsUpdate = true
  if (timeChange.is_changed()) {
    if (timeChange.hourDiff != 0 || timeChange.minuteDiff != 0) {
      rtc::apply_time_delta(timeChange.hourDiff, timeChange.minuteDiff);
    }
    if (timeChange.hourMode != userInput::TimeChange::HourMode::NO_CHANGE) {
      rtc::set_hour_mode(
        timeChange.hourMode == userInput::TimeChange::HourMode::TO_TWELVE
          ? rtc::HourMode::TWELVE
          : rtc::HourMode::TWENTY_FOUR
      );
    }
  }
  


  if (timeChange.is_changed() || rtc::has_minute_passed()) {
    Serial.println("");
    print_change_event_data(timeChange);

    int8_t hour, minute;
    bool isTwentyFourHour;
    bool isPM = rtc::get_time(&hour, &minute, &isTwentyFourHour);
    tube::set_time(hour, minute, isTwentyFourHour);
    tube::update_display();
    rtc::reset_minute_passed();

    print_current_time_state(hour, minute, isTwentyFourHour, isPM);
  }
}

void print_change_event_data(userInput::TimeChange timeChange) {
  if (timeChange.is_changed()) {
    Serial.print("Triggered by user input. timeChange = {hourDiff: ");
    Serial.print(timeChange.hourDiff);
    Serial.print(", minuteDiff: ");
    Serial.print(timeChange.minuteDiff);
    Serial.print(", hourMode: ");
    if (timeChange.hourMode == userInput::TimeChange::HourMode::NO_CHANGE)
      Serial.print("NO_CHANGE");
    else if (timeChange.hourMode == userInput::TimeChange::HourMode::TO_TWELVE)
      Serial.print("TO_TWELVE");
    else
      Serial.print("TO_TWENTY_FOUR");
    Serial.println("}");
  } else {
    Serial.println("Triggered by RTC.");
  }
}

void print_current_time_state(int8_t hour, int8_t minute, bool isTwentyFourHour, bool isPM) {
  Serial.print(hour);
  Serial.print("\t");
  Serial.print(minute);
  Serial.print("\t");
  Serial.print(isPM ? "PM" : "AM");
  Serial.print("\t");
  Serial.println(isTwentyFourHour ? "24h" : "12h");
}


// TODO look into sleeping for power saving!!!
// https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html (not correct for the newer Nano Every?)
// https://forum.arduino.cc/t/help-needed-sleeping-the-atmega4809-arduino-nano-every/914000
// http://ww1.microchip.com/downloads/en/AppNotes/TB3213-Getting-Started-with-RTC-90003213A.pdf (linked from above)
// Ideally we can wake on an interrupt from the external RTC/buttons- need to look into how the RTC works
// https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega4808-4809-Data-Sheet-DS40002173A.pdf#_OPENTOPIC_TOC_PROCESSING_d137e27252


// TODO look into hardware debounce on buttons? (with a capacitor)
