#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <RTClib.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

// Comment this out to use the software RTC
#define USE_HARDWARE_RTC 1

class Timekeeper {
public:
  /**
   * @param gpsTX The GPS transmit pin
   * @param gpsRX The GPS receive pin
   * @param timeSetIntervalSeconds The time, in seconds, between GPS-based clock resynchronizations
   */
  Timekeeper(uint8_t gpsTX, uint8_t gpsRX, uint32_t timeSetIntervalSeconds);

  /**
   * Starts the RTC and GPS
   */
  void begin();

  /**
   * Updates the RTC data, setting the RTC from the GPS if necessary
   */
  void update();

  /**
   * Returns true if the current time has been set by GPS and is valid
   */
  bool isTimeValid() const;

  /**
   * Retrieves the current date/time information
   */
  const DateTime &getTime() const;

  /**
   * Retrieves the number of milliseconds since the last second rollover
   */
  const uint16_t getMilliseconds() const;

  /**
   * Sets the current timezone
   * 
   * @param timezone The timezone offset, WRT GMT
   * @param dst Whether DST is active or not
   */
  void setTimeZone(int8_t timezone, bool dst);

  /**
   * Resets the current time by GPS
   */
  void resetTime();

  /**
   * Gets whether the time is currently being set via GPS
   */
  bool isTimeSetPending();

private:
#ifdef USE_HARDWARE_RTC
  RTC_DS1307 rtc;
#else
  RTC_Millis rtc;
#endif

  SoftwareSerial gpsSerial;
  Adafruit_GPS gps;
  uint32_t timeSetIntervalSeconds;

  uint32_t lastMillis;
  uint32_t currentMillis;

  int8_t timezone;
  bool dst;
  int8_t pendingTimezoneAdjustment;
  bool pendingTimeReset;
  DateTime lastTime;

  uint32_t lastSetTime;


  /**
   * Reads GPS data
   */
  void readGPS();

  /**
   * Sets the current clock time by GPS
   */
  void setClockTime();
};

#endif
