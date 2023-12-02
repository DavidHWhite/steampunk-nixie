#include "Arduino.h"

#include "userInput.h"
#include "pins.h"

// TODO verify that everything starts up fine regardless of the initial state of each button/switch

namespace userInput {
                          // General constants
  constexpr unsigned long INPUT_TIMEOUT        = 10000,
                          // Button-related constants
                          ON_DEBOUNCE_WAIT     = 50,
                          INITIAL_REPEAT_DELAY = 500,
                          FURTHER_REPEAT_DELAY = 100,
                          OFF_DEBOUNCE_WAIT    = 80,
                          // Switch-related constants
                          SWITCH_DEBOUNCE_TIME = 150;

  /*
   * State tracker declarations
   */

  class ButtonState {
    private:
      const int pin;
      enum class State {
        UNPRESSED, DEBOUNCING_ON, FIRED_ONCE, REPEATING, DEBOUNCING_OFF
      } state = State::UNPRESSED;
      unsigned long timeLastAction = 0;
    public:
      ButtonState(int buttonPin) : pin(buttonPin) {};
      bool poll();
  };

  class SwitchState {
    private:
      const int pin;
      enum class State {
        TWELVE, TWENTY_FOUR, DEBOUNCING
      } state = State::TWELVE;
      unsigned long timeStartedBouncing;
    public:
      SwitchState(int switchPin) : pin(switchPin) {};
      TimeChange::HourMode poll();
  };

  /*
   * Publicly accessible operations defined in header
   */

  static unsigned long lastWakingUserInputTime = 0;

  static ButtonState bHourInc(pins::input::HOUR_INC),
                     bHourDec(pins::input::HOUR_DEC),
                     bMinuteInc(pins::input::MIN_INC),
                     bMinuteDec(pins::input::MIN_DEC);
  static SwitchState sHourMode(pins::input::HOUR_FORMAT_SWITCH);

  bool TimeChange::is_changed() {
    return hourDiff != 0 || minuteDiff != 0 || hourMode != HourMode::NO_CHANGE;
  }

  void setup() {
    pinMode(pins::input::HOUR_INC,           INPUT_PULLUP);
    pinMode(pins::input::HOUR_DEC,           INPUT_PULLUP);
    pinMode(pins::input::MIN_INC,            INPUT_PULLUP);
    pinMode(pins::input::MIN_INC,            INPUT_PULLUP);
    pinMode(pins::input::BUTTON_OR,          INPUT_PULLUP);
    pinMode(pins::input::HOUR_FORMAT_SWITCH, INPUT_PULLUP);
  }

  bool has_timed_out() {
    return millis() - lastWakingUserInputTime > INPUT_TIMEOUT;
  }

  TimeChange check_state() {
    // lastWakingUserInputTime is updated within the poll() methods-
    // I don't need to touch it here
    TimeChange result;
    if (bHourInc.poll()) { result.hourDiff += 1; }
    if (bHourDec.poll()) { result.hourDiff -= 1; }
    if (bMinuteInc.poll()) { result.minuteDiff += 1; }
    if (bMinuteDec.poll()) { result.minuteDiff -= 1; }
    result.hourMode = sHourMode.poll();
    return result;
  }

  /*
   * State tracker implementations
   */

  bool ButtonState::poll() {
    bool doFire = false;
    bool doStayAwake = false;
    bool isPressedCurrently = digitalRead(pin);
    unsigned long curTime = millis();
    unsigned long timeSinceChange = curTime - timeLastAction;
    State prevState = state;

    if (isPressedCurrently) {
      doStayAwake = true;
    }

    switch (prevState) {
      case State::UNPRESSED:
        if (isPressedCurrently) {
          state = State::DEBOUNCING_ON;
        }
        break;
      case State::DEBOUNCING_ON:
        if (timeSinceChange >= ON_DEBOUNCE_WAIT) {
          if (isPressedCurrently) {
            state = State::FIRED_ONCE;
            doFire = true;
          } else {
            state = State::UNPRESSED;
          }
        }
        break;
      case State::FIRED_ONCE:
      case State::REPEATING:
        if (isPressedCurrently) {
          if ((prevState == State::FIRED_ONCE && timeSinceChange >= INITIAL_REPEAT_DELAY) ||
              (prevState == State::REPEATING && timeSinceChange >= FURTHER_REPEAT_DELAY)) {
            state = State::REPEATING;
            timeLastAction = curTime;
            doFire = true;
          }
        } else {
          state = State::DEBOUNCING_OFF;
        }
        break;
      default: // State::DEBOUNCING_OFF
        if (timeSinceChange >= OFF_DEBOUNCE_WAIT) {
          state = State::UNPRESSED;
        }
        break;
    }

    if (prevState != state) {
      timeLastAction = curTime;
      doStayAwake = true;
    }

    if (doStayAwake) {
      lastWakingUserInputTime = curTime;
    }

    return doFire;
  }

  TimeChange::HourMode SwitchState::poll() {
    TimeChange::HourMode result = TimeChange::HourMode::NO_CHANGE;
    unsigned long curTime = millis();
    bool isReadingTwelve = digitalRead(pin); // TODO verify which state is 12 vs 24
    State prevState = state;

    switch (prevState) {
      case State::TWELVE:
      case State::TWENTY_FOUR:
        if ((!isReadingTwelve && state == State::TWELVE) ||
            (isReadingTwelve && state == State::TWENTY_FOUR)) {
          state = State::DEBOUNCING;
          timeStartedBouncing = curTime;
        }
        break;
      default: // State::DEBOUNCING
        if (curTime - timeStartedBouncing >= SWITCH_DEBOUNCE_TIME) {
          state = isReadingTwelve ? State::TWELVE : State::TWENTY_FOUR;
          result = isReadingTwelve ? TimeChange::HourMode::TO_TWELVE : TimeChange::HourMode::TO_TWENTY_FOUR;
        }
    }

    if (state != prevState) {
      lastWakingUserInputTime = curTime;
    }
    
    return result;
  }

}