#ifndef CLOCK_FRAME_BUFFERS_H
#define CLOCK_FRAME_BUFFERS_H

#include "FrameBufferView.h"
#include "ClockDisplay.h"

/**
 * Initializes various frame buffers for the clock display
 */
class ClockFrameBuffers {
public:
  /**
   * Initializes a set of clock frame buffers from the given clock display
   */
  ClockFrameBuffers(ClockDisplay &clockDisplay);

  /**
   * Deletes all frame buffers
   */
  ~ClockFrameBuffers();

  // Utility functions to update the underlying buffers
  void updateFade();
  void accelerateFadeToEnd();

  /**
   * The following are simple getters for the frame buffers
   */
  inline FrameBufferView *getSecondBuffer() {
    return secondBuffer;
  }

  inline FrameBufferView *getMinuteBuffer() {
    return minuteBuffer;
  }

  inline FrameBufferView *getHourBuffer() {
    return hourBuffer;
  }

  inline FrameBufferView *getPendulumBuffer() {
    return pendulumBuffer;
  }

  inline FrameBufferView *getFaceInnerRingBuffer() {
    return faceInnerRingBuffer;
  }

  inline FrameBufferView *getFaceOuterRingBuffer() {
    return faceOuterRingBuffer;
  }

  inline FrameBufferView *getDisplayLeftBuffer() {
    return displayLeftBuffer;
  }

  inline FrameBufferView *getDisplayRightBuffer() {
    return displayRightBuffer;
  }

private:
  FrameBufferView *secondBuffer = NULL;
  FrameBufferView *minuteBuffer = NULL;
  FrameBufferView *hourBuffer = NULL;
  FrameBufferView *pendulumBuffer = NULL;
  FrameBufferView *faceInnerRingBuffer = NULL;
  FrameBufferView *faceOuterRingBuffer = NULL;
  FrameBufferView *displayLeftBuffer = NULL;
  FrameBufferView *displayRightBuffer = NULL;
};

#endif
