#include <DS3231.h>

#include "rtc.h"
#include "pins.h"

DS3231 rtcModule; // TODO make static & move into namespace once this isn't being used in loop()
namespace rtc {
  static volatile bool hasMinutePassed = true; // starts true to ensure that display will immediately be updated

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
    // TODO double check that FALLING is correct
  }

  void apply_time_delta(byte hour_delta, byte minute_delta) {
    DateTime currentMoment = RTClib::now();
    if (minute_delta != 0) {
      rtcModule.setSecond(0);
      rtcModule.setMinute(currentMoment.minute() + minute_delta);
    }
    if (hour_delta != 0) {
      rtcModule.setHour(currentMoment.hour() + hour_delta);
    }
  }

  void set_hour_mode(HourMode mode) {
    rtcModule.setClockMode(mode == HourMode::TWELVE);
  }

  void get_time(byte* hour, byte* minute, bool* isTwentyFourHour) {
    DateTime currentMoment = RTClib::now();
    *hour = currentMoment.hour();
    *minute = currentMoment.minute();
    bool h12, hPM;
    rtcModule.getHour(h12, hPM);
    *isTwentyFourHour = !h12;
  }

  bool has_minute_passed() {
    return hasMinutePassed;
  }

  void reset_minute_passed() {
    hasMinutePassed = false;
    rtcModule.checkIfAlarm(1);
  }
}