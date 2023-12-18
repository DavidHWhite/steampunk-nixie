// #include <avr/sleep.h>  // remove if not using

#include "display.h"
#include "rtc.h"
#include "userInput.h"
#include "debug_toggle.h"

void setup() {
#if DEBUG
  Serial.begin(115200);
  Serial.println("\n\n\nBeginning program...");
#endif

  rtc::setup();
  display::setup();
  bool isTwelveHourMode = userInput::setup();
  rtc::set_hour_mode(isTwelveHourMode ? rtc::HourMode::TWELVE : rtc::HourMode::TWENTY_FOUR);
}

void test_loop();

void loop() {
  // Optionally replace loop() with test_loop() at compile time
#if USE_TEST_LOOP
  test_loop();
  return;
#endif

  // Scan user inputs
  const userInput::TimeChange timeChange = userInput::check_state();

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

  // If the time has changed, send the new time to the display
  if (timeChange.is_changed() || rtc::has_minute_passed()) {
#if DEBUG
    Serial.println("");
    print_change_event_data(timeChange);
#endif
    int8_t hour, minute;
    rtc::HourMode hourMode;
    bool isPM = rtc::get_time(&hour, &minute, &hourMode);
    (void) isPM; // supress unused variable warning
    display::set_time_display(hour, minute, hourMode == rtc::HourMode::TWENTY_FOUR);
    rtc::reset_minute_passed();
#if DEBUG
    print_current_time_state(hour, minute, hourMode, isPM);
#endif
  }
}
  
  /*
   * Helper functions
   */

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

void print_current_time_state(int8_t hour, int8_t minute, rtc::HourMode hourMode, bool isPM) {
  Serial.print(hour);
  Serial.print("\t");
  Serial.print(minute);
  Serial.print("\t");
  Serial.print(isPM ? "PM" : "AM");
  Serial.print("\t");
  Serial.println(hourMode == rtc::HourMode::TWENTY_FOUR ? "24h" : "12h");
}

void test_loop() {
  Serial.println("\n1:23\t12");
  display::set_time_display(1, 23, false);
  delay(5000);
  Serial.println("\n45:67\t24");
  display::set_time_display(45, 67, true);
  delay(5000);
  Serial.println("\n89:10\t24");
  display::set_time_display(89, 10, true);
  delay(5000);
}


// Look into sleeping for power saving?
// https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html (not correct for the newer Nano Every?)
// https://forum.arduino.cc/t/help-needed-sleeping-the-atmega4809-arduino-nano-every/914000
// http://ww1.microchip.com/downloads/en/AppNotes/TB3213-Getting-Started-with-RTC-90003213A.pdf (linked from above)
// https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega4808-4809-Data-Sheet-DS40002173A.pdf#_OPENTOPIC_TOC_PROCESSING_d137e27252
// While testing, indicate sleep with the onboard LED
