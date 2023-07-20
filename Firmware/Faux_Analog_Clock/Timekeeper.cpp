#include "Arduino.h"
#include "Timekeeper.h"
#include <RTClib.h>

Timekeeper::Timekeeper(uint8_t gpsTX, uint8_t gpsRX, uint32_t timeSetIntervalSeconds) : gpsSerial(gpsTX, gpsRX), gps(&gpsSerial) {
  this->timeSetIntervalSeconds = timeSetIntervalSeconds;
}

void Timekeeper::begin() {
  // Init RTC
#ifdef USE_HARDWARE_RTC
  rtc.begin();
#else
  rtc.begin(DateTime(static_cast<uint32_t>(0)));
#endif

  // Init GPS
  gps.begin(9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ);

  // Init milliseconds
  lastMillis = currentMillis = millis();
  lastSetTime = 0;

  // Init timezone
  timezone = -6;
  dst = true;
  pendingTimezoneAdjustment = 0;
  pendingTimeReset = true;
}


void Timekeeper::update() {
  // Set the clock the time is invalid or it's been awhile since the last set
  pendingTimeReset = pendingTimeReset || lastSetTime == 0 || (lastTime.unixtime() - lastSetTime >= timeSetIntervalSeconds);
  if (pendingTimeReset) {
    setClockTime();
  }

  // Update time
  uint8_t lastSecond = lastTime.second();
  lastTime = rtc.now();

  // Update milliseconds
  uint32_t nowMillis = millis();
  if (lastTime.second() != lastSecond) {
    lastMillis = nowMillis;

    // Apply pending timezone adjustment only just as seconds are changing
    if (pendingTimezoneAdjustment != 0) {
      TimeSpan timezoneOffset(0, pendingTimezoneAdjustment, 0, 0);
      lastTime = lastTime + timezoneOffset;
      rtc.adjust(lastTime);
      pendingTimezoneAdjustment = 0;
      lastSetTime = lastTime.unixtime();
    }
  }
  currentMillis = nowMillis;
}


bool Timekeeper::isTimeValid() const {
  return lastSetTime > 0;
}

const DateTime &Timekeeper::getTime() const {
  return lastTime;
}

const uint16_t Timekeeper::getMilliseconds() const {
  return static_cast<uint16_t>(currentMillis - lastMillis) % 1000;
}

void Timekeeper::setTimeZone(int8_t timezone, bool dst) {
  pendingTimezoneAdjustment += timezone - this->timezone + (dst ? 1 : 0) - (this->dst ? 1 : 0);
  this->timezone = timezone;
  this->dst = dst;
}

void Timekeeper::resetTime() {
  pendingTimeReset = true;
}

bool Timekeeper::isTimeSetPending() {
  return pendingTimeReset;
}


void Timekeeper::readGPS() {
  while (gps.available()) {
    gps.read();
  }
}

void Timekeeper::setClockTime() {
  // Read the GPS
  if (!gpsSerial.isListening()) {
    gpsSerial.listen();
  }
  readGPS();

  // Set the time
  if (gps.newNMEAreceived()) {
    if (gps.parse(gps.lastNMEA()) && gps.fix && gps.year > 0) {
      TimeSpan timezoneOffset(0, timezone + (dst ? 1 : 0), 0, 0);
      lastTime = DateTime(gps.year, gps.month, gps.day, gps.hour, gps.minute, gps.seconds) + timezoneOffset;
      rtc.adjust(lastTime);
      pendingTimezoneAdjustment = 0;

      lastSetTime = lastTime.unixtime();
      pendingTimeReset = false;

      // Stop the GPS serial port from listening.
      // I wish there were a better way than this, but SoftwareSerial does not provide an end() method.
      SoftwareSerial dummySerial(0, 0);
      dummySerial.begin(1200);
      dummySerial.listen();
    }
  }
}
