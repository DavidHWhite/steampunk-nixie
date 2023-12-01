/*

*/

#include <avr/sleep.h>

#include "tube.h"
#include "rtc.h"
#include "userInput.h"

// TODO while testing, indicate sleep with the onboard LED

void setup() {
  // TODO setup RTC first- if it already has a stored time, send that as an argument to tube::setup()
  tube::setup();
  // TODO setup userInput

  // TODO enable interrupt on ITR pin (RISING?)

  // TODO don't forget global interrupt enable!!!
}

// This becomes true in the ISRs for the user inputs to wake the chip from sleep
// TODO don't forget this! ^
// This is set to false once it has been acted on.
static volatile bool userInterrupted = false;

// This value is set to true whenever the ISR for the RTC is triggered
// TODO don't forget this! ^
// OR when the input module indicates that the user has changed a value.
// It is set to false once the display has been updated.
static volatile bool displayNeedsUpdate = true;

// TODO move these into userInput.cpp


void loop() {
  if (userInput::timed_out()) {
    // TODO enable input interrupts (RTC INT should already be on)
    // TODO enter sleep mode
  }

  // At this point, either a minute has passed or a user input was triggered.

  // If user input was detected or a button is being held down, update the recorded time.
  if (userInterrupted || userInput::any_button_pressed()) {
    userInput::check_state(); // TODO store return value
  }

  // TODO communicate with RTC

  // TODO update display





  // Evaluate buttons
  //   '-> Time modification data
  
  //   v Time modification data
  // Communicate with RTC (only if it's been at least 900ms or a modification is needed)
  //   '-> Display change data
  
  //   v Display change data
  // Update display (only if something has changed)
}


  // TODO look into sleeping for power saving!!!
  // https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html (not correct for the newer Nano Every?)
  // https://forum.arduino.cc/t/help-needed-sleeping-the-atmega4809-arduino-nano-every/914000
  // http://ww1.microchip.com/downloads/en/AppNotes/TB3213-Getting-Started-with-RTC-90003213A.pdf (linked from above)
  // Ideally we can wake on an interrupt from the external RTC/buttons- need to look into how the RTC works
  // https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega4808-4809-Data-Sheet-DS40002173A.pdf#_OPENTOPIC_TOC_PROCESSING_d137e27252


  // TODO look into hardware debounce on buttons? (with a capacitor)

