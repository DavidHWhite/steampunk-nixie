// #include <avr/sleep.h>  // remove if not using

#include "display.h"
#include "rtc.h"
#include "userInput.h"
#include "debug_toggle.h"

void setup() {
#if DEBUG
  Serial.begin(115200);
  Serial.println(F("\n\n\nBeginning program..."));
#endif

  rtc::setup(rtc::Granularity::SECONDS);
  display::setup();
  bool isTwelveHourMode = userInput::setup();
  rtc::set_hour_mode(isTwelveHourMode ? rtc::HourMode::TWELVE : rtc::HourMode::TWENTY_FOUR);

  // TODO remove
  display::set_display_digits(
    display::get_displayVal(2),
    display::get_displayVal(9),
    display::get_displayVal(5),
    display::get_displayVal(9),
    display::get_displayVal(5),
    display::get_displayVal(9)
  );
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
  if (timeChange.is_changed() || rtc::has_time_passed()) {
#if DEBUG
    Serial.println(F(""));
    print_change_event_data(timeChange);
#endif
    int8_t hour, minute, second;
    rtc::HourMode hourMode;
    bool isPM = rtc::get_time(&hour, &minute, &second, &hourMode);
    (void) isPM; // supress unused variable warning
    display::set_time_display(hour, minute, second, hourMode == rtc::HourMode::TWENTY_FOUR);
    rtc::reset_time_passed();
#if DEBUG
    print_current_time_state(hour, minute, second, hourMode, isPM);
#endif
  }
}
  
  /*
   * Helper functions
   */

#if DEBUG
void print_change_event_data(userInput::TimeChange timeChange) {
  if (timeChange.is_changed()) {
    Serial.print(F("Triggered by user input. timeChange = {hourDiff: "));
    Serial.print(timeChange.hourDiff);
    Serial.print(F(", minuteDiff: "));
    Serial.print(timeChange.minuteDiff);
    Serial.print(F(", hourMode: "));
    if (timeChange.hourMode == userInput::TimeChange::HourMode::NO_CHANGE)
      Serial.print(F("NO_CHANGE"));
    else if (timeChange.hourMode == userInput::TimeChange::HourMode::TO_TWELVE)
      Serial.print(F("TO_TWELVE"));
    else
      Serial.print(F("TO_TWENTY_FOUR"));
    Serial.println(F("}"));
  } else {
    Serial.println(F("Triggered by RTC."));
  }
}
#endif

#if DEBUG
void print_current_time_state(int8_t hour, int8_t minute, int8_t second, rtc::HourMode hourMode, bool isPM) {
  Serial.print(hour);
  Serial.print(F("\t"));
  Serial.print(minute);
  Serial.print(F("\t"));
  Serial.print(second);
  Serial.print(F("\t"));
  Serial.print(isPM ? F("PM") : F("AM"));
  Serial.print(F("\t"));
  Serial.println(hourMode == rtc::HourMode::TWENTY_FOUR ? F("24h") : F("12h"));
}
#endif

#if USE_TEST_LOOP
void test_loop() {
  Serial.println(F("\n1:23\t12"));
  display::set_time_display(1, 23, false);
  delay(5000);
  Serial.println(F("\n45:67\t24"));
  display::set_time_display(45, 67, true);
  delay(5000);
  Serial.println(F("\n89:10\t24"));
  display::set_time_display(89, 10, true);
  delay(5000);
}
#endif


// Look into sleeping for power saving?
// https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html (not correct for the newer Nano Every?)
// https://forum.arduino.cc/t/help-needed-sleeping-the-atmega4809-arduino-nano-every/914000
// http://ww1.microchip.com/downloads/en/AppNotes/TB3213-Getting-Started-with-RTC-90003213A.pdf (linked from above)
// https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega4808-4809-Data-Sheet-DS40002173A.pdf#_OPENTOPIC_TOC_PROCESSING_d137e27252
// While testing, indicate sleep with the onboard LED
