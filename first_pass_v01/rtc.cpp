#include <DS3231.h>

#include "rtc.h"
#include "pins.h"
#include "debug_toggle.h"

namespace rtc {
  static DS3231 rtcModule;
  static volatile bool hasMinutePassed = true;
  // hasMinutePassed starts true to ensure that display will immediately be updated

  /*
   * ISR to allow the buttons and switch to wake the MCU from sleep
   */

  void rtc_isr() {
    hasMinutePassed = true;
  }

  /*
   * Publicly accessible RTC operations declared in header
   */

  void setup() {
    pinMode(pins::rtc::INT, INPUT_PULLUP);
    pinMode(pins::rtc::O32K, INPUT_PULLUP);

    Wire.begin();

    // Assign parameter values for Alarm 1
    byte alarmDay = 0;
    byte alarmHour = 0;
    byte alarmMinute = 0;
    byte alarmSecond = 0;
    byte alarmBits = 0b00001110; // Alarm 1 every minute
    bool alarmDayIsDay = false;
    bool alarmH12 = false;
    bool alarmPM = false;

    // Set alarm 1 to fire at one-second intervals
    rtcModule.turnOffAlarm(1);
    rtcModule.setA1Time(
       alarmDay, alarmHour, alarmMinute, alarmSecond,
       alarmBits, alarmDayIsDay, alarmH12, alarmPM);
    // enable Alarm 1 interrupts
    rtcModule.turnOnAlarm(1);
    // clear Alarm 1 flag
    rtcModule.checkIfAlarm(1);

    // When using interrupt with only one of the DS3231 alarms, as in this example,
    // it may be advisable to prevent the other alarm entirely,
    // so it will not covertly block the outgoing interrupt signal.

    // Prevent Alarm 2 altogether by assigning a 
    // nonsensical alarm minute value that cannot match the clock time,
    // and an alarmBits value to activate "when minutes match".
    alarmMinute = 0xFF; // a value that will never match the time
    alarmBits = 0b01100000; // Alarm 2 when minutes match, i.e., never
    
    // Upload the parameters to prevent Alarm 2 entirely
    rtcModule.setA2Time(
        alarmDay, alarmHour, alarmMinute,
        alarmBits, alarmDayIsDay, alarmH12, alarmPM);
    // disable Alarm 2 interrupt
    rtcModule.turnOffAlarm(2);
    // clear Alarm 2 flag
    rtcModule.checkIfAlarm(2);

    // Set up an interrupt every minute from the SQW/INT pin
    rtcModule.enableOscillator(false, false, 0);
    attachInterrupt(digitalPinToInterrupt(pins::rtc::INT), rtc_isr, FALLING);
  }

  static inline int8_t h12_to_h24(int8_t hour, bool isPM) {
    int8_t result = hour;
    if (hour == 12 && !isPM) {
      result = 0;
    } else if (hour != 12 && isPM) {
      result += 12;
    }
    return result;
  }

  void apply_time_delta(int8_t hourDelta, int8_t minuteDelta) {
    bool h12, hPM = false;
    int8_t oldHour = rtcModule.getHour(h12, hPM);
    int8_t oldMinute = rtcModule.getMinute();

    if (h12) {
      oldHour = h12_to_h24(oldHour, hPM);
    }
    int8_t newHour = (24 + oldHour + hourDelta) % 24;
    int8_t newMinute = (60 + oldMinute + minuteDelta) % 60;
    if (minuteDelta != 0) {
      rtcModule.setSecond(0);
    }
    rtcModule.setHour(newHour);
    rtcModule.setMinute(newMinute);
  }

  void set_hour_mode(HourMode newMode) {
    HourMode oldMode;
    int8_t hour, minute;
    bool isPM = get_time(&hour, &minute, &oldMode);
    if (oldMode != newMode) {
      rtcModule.setClockMode(newMode == HourMode::TWELVE);
      if (oldMode == HourMode::TWELVE) {
        hour = h12_to_h24(hour, isPM);
      }
      rtcModule.setHour(hour);
    }
  }

  bool get_time(int8_t* o_hour, int8_t* o_minute, HourMode* o_hourMode) {
    bool h12, hPM;
    *o_hour = rtcModule.getHour(h12, hPM);
    *o_hourMode = h12 ? HourMode::TWELVE : HourMode::TWENTY_FOUR;
    *o_minute = rtcModule.getMinute();
    return hPM;
  }

  bool has_minute_passed() {
    return hasMinutePassed;
  }

  void reset_minute_passed() {
    hasMinutePassed = false;
    rtcModule.checkIfAlarm(1);
  }
}