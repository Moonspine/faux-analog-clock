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
   * Sets all values in the frame buffer
   * 
   * @param value The new value
   */
  void setAllValues(uint8_t value);

  /**
   * Sets this buffer up to be fadeable by calling the updateFade() method
   * 
   * @param targetFadeValue The value to which this buffer will fade all elements
   * @param fadeRate The value added to / subtracted from elements on each fade tick
   * @param fadeTickDelay The delay between fade ticks, in number of calls to updateFade()
   */
  void initializeFade(uint8_t fadeRate, uint16_t fadeTickDelay, uint8_t targetFadeValue);
  
  /**
   * Sets this buffer up to be fadeable by calling the updateFade() method
   * 
   * @param fadeRate The value added to / subtracted from elements on each fade tick
   * @param fadeTickDelay The delay between fade ticks, in number of calls to updateFade()
   */
  void initializeFade(uint8_t fadeRate, uint16_t fadeTickDelay);

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

  uint8_t targetFadeValue;
  uint8_t fadeRate;
  uint16_t fadeTickDelay;
  
  uint16_t remainingFadeTickDelay;
  bool isFadeActive;
};

#endif
