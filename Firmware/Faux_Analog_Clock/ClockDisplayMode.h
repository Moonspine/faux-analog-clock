#ifndef CLOCK_DISPLAY_MODE_H
#define CLOCK_DISPLAY_MODE_H

#include <RTClib.h>
#include "ClockFrameBuffers.h"
#include "ClockFrameBuffers.h"

/**
 * Base class of all clock display modes
 */
class ClockDisplayMode {
public:
  /**
   * Initializes the display mode
   */
  virtual void initialize(ClockFrameBuffers &frameBuffers, uint8_t brightness);

  /**
   * Updates the display mode with the current time
   */
  virtual void update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t pendulumIndex, uint8_t brightness);
};

/**
 * Basic analog clock face
 */
class AnalogClockDisplayMode : public ClockDisplayMode {
public:
  void update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness);
};

/**
 * Binary clock face which shows the time using 6 bits for seconds, minutes and 4 bits for hours
 */
class BinaryClockDisplayMode : public ClockDisplayMode {
public:
  void update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness);
};

/**
 * Inverted analog clock face
 */
class InvertedAnalogClockDisplayMode : public ClockDisplayMode {
public:
  void initialize(ClockFrameBuffers &frameBuffers, uint8_t brightness);
  void update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness);
};

/**
 * Ring fills up until rollover
 */
class FillClockDisplayMode : public ClockDisplayMode {
public:
  void update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness);
};

/**
 * Ring fills up until rollover, then unfills
 */
class FillUnfillClockDisplayMode : public ClockDisplayMode {
public:
  void update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness);
};

#endif
