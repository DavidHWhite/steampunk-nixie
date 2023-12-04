#include <avr/sleep.h>

#include "tube.h"
#include "rtc.h"
#include "userInput.h"

// TODO while testing, indicate sleep with the onboard LED

void setup() {
  // TODO setup RTC first- if it already has a stored time, send that as an argument to tube::setup()
  tube::setup();
  // TODO setup userInput

  // TODO check state of 12/24 switch and write to the RTC accordingly

  // TODO add pulldown resistors to pins::anode::RESET_INV and pins::cathode::RESET_INV


  // TODO Option: instead of enabling/disabling interrupts when entering & exiting sleep:
    // Have all three interrupts always active
    // 1. RTC INT sets displayNeedsUpdate
          // Rising edge only
    // 2. BUTTON_OR just wakes the MCU
          // Both edges
    // 3. SWITCH just wakes the MCU 
          // Both edges
  
  interrupts();
}

void loop() {
  if (userInput::has_timed_out()) {
    // TODO enter sleep mode
  }

  // At this point, either a minute has passed or a user input was triggered.
  // Scan user inputs
  // (If needed, this will update the timeout timer stored in userInput)
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

  // If the display needs updating:
  if (timeChange.is_changed() || rtc::has_minute_passed()) {
    rtc::reset_minute_passed();
    int hour, minute;
    bool isTwentyFourHour;
    rtc::get_time(&hour, &minute, &isTwentyFourHour);
    tube::set_time(hour, minute, isTwentyFourHour);
    tube::update_display();
  }
}


  // TODO look into sleeping for power saving!!!
  // https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html (not correct for the newer Nano Every?)
  // https://forum.arduino.cc/t/help-needed-sleeping-the-atmega4809-arduino-nano-every/914000
  // http://ww1.microchip.com/downloads/en/AppNotes/TB3213-Getting-Started-with-RTC-90003213A.pdf (linked from above)
  // Ideally we can wake on an interrupt from the external RTC/buttons- need to look into how the RTC works
  // https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega4808-4809-Data-Sheet-DS40002173A.pdf#_OPENTOPIC_TOC_PROCESSING_d137e27252


  // TODO look into hardware debounce on buttons? (with a capacitor)

