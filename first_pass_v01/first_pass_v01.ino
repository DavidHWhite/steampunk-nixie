/*

*/

#include <avr/sleep.h>

#include "tube.h"
#include "rtc.h"
#include "userInput.h"









// TODO
// TODO
// TODO
// TODO need to be able to disable a single tube for the hour10s digit











// TODO while testing, indicate sleep with the onboard LED

void setup() {
  // TODO setup RTC first- if it already has a stored time, send that as an argument to tube::setup()
  tube::setup();
  // TODO setup userInput

  // TODO enable interrupt on ITR pin (RISING?)

  // TODO don't forget global interrupt enable!!!

  // TODO check state of 12/24 switch and write to the RTC accordingly


  // TODO Option: instead of enabling/disabling interrupts when entering & exiting sleep:
    // Have all three interrupts always active
    // 1. RTC INT sets displayNeedsUpdate
          // Rising edge only
    // 2. BUTTON_OR just wakes the MCU
          // Both edges
    // 3. SWITCH just wakes the MCU 
          // Both edges
}

// This value is set to true whenever the ISR for the RTC is triggered
// TODO don't forget this! ^
// OR when the input module indicates that the user has changed a value.
// It is set to false once the display has been updated.
static volatile bool displayNeedsUpdate = true;

void loop() {
  if (userInput::has_timed_out()) {
    // TODO enable input interrupts (RTC INT should already be on)
    // TODO enter sleep mode
  }

  // At this point, either a minute has passed or a user input was triggered.
  // Scan user inputs
  // (If needed, this will update the timeout timer stored in userInput)
  userInput::TimeChange timeChange = userInput::check_state();

  // If something needs to be written to the RTC, do so
  // Also set displayNeedsUpdate = true
  if (timeChange.is_changed()) {
    displayNeedsUpdate = true;
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
  if (displayNeedsUpdate) {
    displayNeedsUpdate = false;
    
    // Read the time from the RTC
    int hour, minute;
    bool isTwentyFourHour;
    rtc::get_time(&hour, &minute, &isTwentyFourHour);

    // Write the time to the display
    tube::set_time(hour, minute, isTwentyFourHour);

    // Update the display
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

