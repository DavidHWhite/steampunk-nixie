/*

*/

#include <avr/sleep.h>

#include "tube.h"
#include "rtc.h"

void setup() {
  // TODO setup RTC first- if it already has a stored time, send that as an argument to setup_tube
  tube::setup();
}

void loop() {
  // Evaluate buttons
  //  '-> Time modification data
  
  //  v'' Time modification data
  // Communicate with RTC (only if it's been at least 900ms or a modification is needed)
  //  '-> Display change data
  
  //  v'' Display change data
  // Update display (only if something has changed)
  
  
  
  
  
  
  
  
  
  delay(50);
}


  // TODO look into sleeping for power saving!!!
  // https://www.nongnu.org/avr-libc/user-manual/group__avr__sleep.html (not correct for the newer Nano Every?)
  // https://forum.arduino.cc/t/help-needed-sleeping-the-atmega4809-arduino-nano-every/914000
  // http://ww1.microchip.com/downloads/en/AppNotes/TB3213-Getting-Started-with-RTC-90003213A.pdf (linked from above)
  // Ideally we can wake on an interrupt from the external RTC/buttons- need to look into how the RTC works

  // TODO look into hardware debounce on buttons? (with a capacitor)


  // This psuedocode assumes we're not using interrupts
  bool redraw = false;
  // Check button state
    // If any buttons have been pressed: handle them (inc. a change in the 12/24 switch)
    // ...
    // redraw = true;
  // Get {hour, minute} struct from RTC, compare to old {hour, minute}
    // If changed:
      // redraw = true;
  // If redraw:
    // Send new 
