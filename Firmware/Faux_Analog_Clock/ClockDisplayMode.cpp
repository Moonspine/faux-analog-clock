#include <RTClib.h>
#include "ClockFrameBuffers.h"
#include "ClockDisplayMode.h"
#include "Pendulum.h"

void ClockDisplayMode::initialize(ClockFrameBuffers &frameBuffers, uint8_t brightness) {
  // Initialize basic fade targets which most implementations will use
  frameBuffers.getPendulumBuffer()->setFadeTarget(0);
  frameBuffers.getSecondBuffer()->setFadeTarget(0);
  frameBuffers.getMinuteBuffer()->setFadeTarget(0);
  frameBuffers.getHourBuffer()->setFadeTarget(0);
}

void ClockDisplayMode::update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness) {
  // The base class only handle pendulum updates (this feature is identical for most implementations)
  frameBuffers.getPendulumBuffer()->setValue(pgm_read_byte(PENDULUM_LED_INDEX + milliseconds), brightness);
}


void AnalogClockDisplayMode::update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness) {
  ClockDisplayMode::update(frameBuffers, now, milliseconds, brightness);
  frameBuffers.getSecondBuffer()->setValue(now.second(), brightness);
  frameBuffers.getMinuteBuffer()->setValue(now.minute(), brightness);
  frameBuffers.getHourBuffer()->setValue(now.hour() % 12, brightness);
}


void BinaryClockDisplayMode::update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness) {
  ClockDisplayMode::update(frameBuffers, now, milliseconds, brightness);
  frameBuffers.getSecondBuffer()->setValuesBinaryDisplay(now.second(), 6, 10, false, brightness);
  frameBuffers.getMinuteBuffer()->setValuesBinaryDisplay(now.minute(), 6, 10, false, brightness);
  frameBuffers.getHourBuffer()->setValuesBinaryDisplay(now.hour() % 12, 4, 3, false, brightness);
}

void InvertedAnalogClockDisplayMode::initialize(ClockFrameBuffers &frameBuffers, uint8_t brightness) {
  frameBuffers.getPendulumBuffer()->setFadeTarget(brightness);
  frameBuffers.getSecondBuffer()->setFadeTarget(brightness);
  frameBuffers.getMinuteBuffer()->setFadeTarget(brightness);
  frameBuffers.getHourBuffer()->setFadeTarget(brightness);
}

void InvertedAnalogClockDisplayMode::update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness) {
  frameBuffers.getPendulumBuffer()->setValue(pgm_read_byte(PENDULUM_LED_INDEX + milliseconds), 0);
  frameBuffers.getSecondBuffer()->setValue(now.second(), 0);
  frameBuffers.getMinuteBuffer()->setValue(now.minute(), 0);
  frameBuffers.getHourBuffer()->setValue(now.hour() % 12, 0);
}

/**
 * Draws a filled line from the first item in a buffer, preserving fade
 * 
 * @param frameBuffer The buffer to render into
 * @param ringValue The ring value (i.e. how many values to fill)
 * @param brightness The filled brightness
 */
inline void drawFilledLine(FrameBufferView *frameBuffer, uint8_t ringValue, uint8_t brightness) {
  if (ringValue == 0) {
    frameBuffer->setFadeTarget(0);
  } else {
    frameBuffer->setFadeTarget(brightness);
    frameBuffer->setValues(ringValue, 60, 0);
  }
}

/**
 * Draws an ufilled line from the first item in a buffer, preserving fade
 * 
 * @param frameBuffer The buffer to render into
 * @param ringValue The ring value (i.e. how many values to fill)
 * @param brightness The filled brightness
 */
inline void drawUnfilledLine(FrameBufferView *frameBuffer, uint8_t ringValue, uint8_t brightness) {
  if (ringValue == 0) {
    frameBuffer->setFadeTarget(brightness);
  } else {
    frameBuffer->setFadeTarget(0);
    frameBuffer->setValues(ringValue, 60, brightness);
  }
}

void FillClockDisplayMode::update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness) {
  ClockDisplayMode::update(frameBuffers, now, milliseconds, brightness);
  drawFilledLine(frameBuffers.getSecondBuffer(), now.second(), brightness);
  drawFilledLine(frameBuffers.getMinuteBuffer(), now.minute(), brightness);
  drawFilledLine(frameBuffers.getHourBuffer(), now.hour() % 12, brightness);
}

void FillUnfillClockDisplayMode::update(ClockFrameBuffers &frameBuffers, const DateTime &now, const uint16_t milliseconds, uint8_t brightness) {
  ClockDisplayMode::update(frameBuffers, now, milliseconds, brightness);

  if (now.minute() % 2 == 0) {
    drawFilledLine(frameBuffers.getSecondBuffer(), now.second(), brightness);
  } else {
    drawUnfilledLine(frameBuffers.getSecondBuffer(), now.second(), brightness);
  }
  
  if (now.hour() % 2 == 0) {
    drawFilledLine(frameBuffers.getMinuteBuffer(), now.minute(), brightness);
  } else {
    drawUnfilledLine(frameBuffers.getMinuteBuffer(), now.minute(), brightness);
  }

  if (now.hour() < 12) {
    drawFilledLine(frameBuffers.getHourBuffer(), now.hour(), brightness);
  } else {
    drawUnfilledLine(frameBuffers.getHourBuffer(), now.hour() % 12, brightness);
  }
}
