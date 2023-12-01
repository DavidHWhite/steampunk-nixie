#include "userInput.h"

namespace userInput {
  constexpr unsigned long INPUT_TIMEOUT_MILLIS = 10000, // TODO play with these
                          INITIAL_REPEAT_DELAY = 500,
                          FURTHER_REPEAT_DELAY = 100;


  static unsigned long buttonPressedTime[4] = {0, 0, 0, 0};
  static bool buttonPressedStatus[4] = {false, false, false, false};
  static unsigned long lastWakingUserInputTime = 0;

  bool any_button_pressed() {
    // TODO implement this
    return false;
  }

  bool timed_out() {
    return millis() - lastWakingUserInputTime > INPUT_TIMEOUT_MILLIS;
  }

  void check_state() { // TODO change return type; needs to communicate necessary changes
    // TODO check for newly pressed switches
      // If found, update buttonPressedStatus and buttonPressedTime then indicate the necessary time change
    // TODO if any of the buttons are *pressed* and have been pressed for a sufficiently long time then indicate the necessary time change
      // Then change buttonPressedTime so that there's a slight delay before processing that button again
  }
}