#ifndef FRAME_BUFFER_VIEW
#define FRAME_BUFFER_VIEW

#include "Arduino.h"

class FrameBufferView {
public:
  /**
   * Creates a frame buffer view which operates on the given frame buffer data
   * 
   * @param frameBuffer The frame buffer data
   * @param count The number of items in the frame buffer
   */
  FrameBufferView(uint8_t *frameBuffer, uint8_t count);

  /**
   * Sets the value at the given index in the frame buffer
   * 
   * @param index The index to set
   * @param value The value to set
   */
  void setValue(uint8_t index, uint8_t value);

  /**
   * Sets multiple values at once
   * 
   * @param startIndex The first index to set
   * @param valueCount The number of values to set
   * @param value The value to set
   */
  void setValues(uint8_t startIndex, uint8_t valueCount, uint8_t value);

  /**
   * Sets all values in the frame buffer
   * 
   * @param value The new value
   */
  void setAllValues(uint8_t value);

  /**
   * Sets the values in the buffer as a binary display for the given value
   * 
   * @param bitValue The value to display (decomposed into bits)
   * @param bitCount The number of bits to display (between 1 and 8, inclusive)
   * @param valuesPerBit The number of buffer values to set for each bit (between 1 and (this->count / bitCount), inclusive)
   * @param setZeroes If true, zeroes are set (false for better fade effects)
   * @param intensity The intensity of the display (the value to set for each frame buffer element)
   */
  void setValuesBinaryDisplay(uint8_t bitValue, uint8_t bitCount, uint8_t valuesPerBit, bool setZeroes, uint8_t intensity);

  /**
   * Sets this buffer up to be fadeable by calling the updateFade() method
   * 
   * @param microsecondsPerFadeTick The number of microseconds it takes to fade the vlaues by 1
   * @param targetFadeValue The value to which this buffer will fade all elements
   */
  void initializeFade(uint32_t microsecondsPerFadeTick, uint8_t targetFadeValue);
  
  /**
   * Sets this buffer up to be fadeable by calling the updateFade() method
   * 
   * @param microsecondsPerFadeTick The number of microseconds it takes to fade the vlaues by 1
   */
  void initializeFade(uint32_t microsecondsPerFadeTick);

  /**
   * Updates the fade target without resetting any internal fade timers
   * 
   * @param targetFadeValue The value to which this buffer will fade all elements
   */
  void setFadeTarget(uint8_t targetFadeValue);

  /**
   * Updates the buffer fade if required
   */
  void updateFade();

  /**
   * Instantly sets all frame buffer values to the target fade value
   */
  void accelerateFadeToEnd();

private:
  uint8_t *frameBuffer;
  uint8_t count;

  uint32_t microsecondsPerFadeTick;
  uint8_t targetFadeValue;
  
  bool isFadeActive;
  bool lastFadeActive;
  uint32_t lastFadeTimestamp;
};

#endif
