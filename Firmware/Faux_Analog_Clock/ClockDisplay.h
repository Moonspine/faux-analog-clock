#ifndef CLOCK_DISPLAY_H
#define CLOCK_DISPLAY_H

#include "FrameBufferView.h"

const uint8_t CLOCK_DISPLAY_LED_COUNT = 182;
const uint8_t CLOCK_DISPLAY_PIN_COUNT = 14;

const uint8_t CLOCK_SECONDS_OFFSET = 0;
const uint8_t CLOCK_MINUTES_OFFSET = 60;
const uint8_t CLOCK_HOURS_OFFSET = 120;
const uint8_t CLOCK_PENDULUM_OFFSET = 132;
const uint8_t CLOCK_FACE_INNER_OFFSET = 144;
const uint8_t CLOCK_FACE_OUTER_OFFSET = 156;
const uint8_t CLOCK_7SEG_LEFT_OFFSET = 168;
const uint8_t CLOCK_7SEG_RIGHT_OFFSET = 175;

/**
 * Charlieplexed clock display using 14 I/O lines to control 182 LEDs.
 * The I/O pins used for this are hard-coded:
 * 
 * PORTD 0..7
 * PORTB 0..3
 * PORTC 0..1
 */
class ClockDisplay {
public:
  /**
   * Basic clock display constructor
   */
  ClockDisplay();

  /**
   * Begins the clock display, setting up I/O pins
   */
  void begin();

  /**
   * Displays the clock LEDs for one frame's duration
   */
  void display();

  /**
   * Sets the LED value at the given index.
   */
  void setLEDValue(uint8_t index, uint8_t value);

  /**
   * Sets multiple LED values by copying them from an array of values
   * 
   * @param source The array of values to copy
   * @param destIndex The index to copy into
   * @param count The number of values to copy
   */
  void setLEDValues(uint8_t *source, uint8_t destIndex, uint8_t count);

  /**
   * Sets all LEDs to the same value
   */
  void setAllLEDValues(uint8_t value);

  /**
   * Creates a view of the internal frame buffer
   * NOTE: The caller is responsible for deleting this view!
   * 
   * @param startIndex The first index of the view
   * @param count The number of bytes to view
   */
  FrameBufferView *newFrameBufferView(uint8_t startIndex, uint8_t count);

private:
  uint8_t frameBuffer[CLOCK_DISPLAY_LED_COUNT];

  /**
   * Waits for the specified time frame
   */
  void timedWait(uint8_t timeFrame);
};

#endif
