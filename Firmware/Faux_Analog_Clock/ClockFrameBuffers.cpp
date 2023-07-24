#include "FrameBufferView.h"
#include "ClockDisplay.h"
#include "ClockFrameBuffers.h"

ClockFrameBuffers::ClockFrameBuffers(ClockDisplay &clockDisplay) {
  secondBuffer = clockDisplay.newFrameBufferView(CLOCK_SECONDS_OFFSET, 60);
  minuteBuffer = clockDisplay.newFrameBufferView(CLOCK_MINUTES_OFFSET, 60);
  hourBuffer = clockDisplay.newFrameBufferView(CLOCK_HOURS_OFFSET, 12);
  pendulumBuffer = clockDisplay.newFrameBufferView(CLOCK_PENDULUM_OFFSET, 12);
  faceInnerRingBuffer = clockDisplay.newFrameBufferView(CLOCK_FACE_INNER_OFFSET, 12);
  faceOuterRingBuffer = clockDisplay.newFrameBufferView(CLOCK_FACE_OUTER_OFFSET, 12);
  displayLeftBuffer = clockDisplay.newFrameBufferView(CLOCK_7SEG_LEFT_OFFSET, 7);
  displayRightBuffer = clockDisplay.newFrameBufferView(CLOCK_7SEG_RIGHT_OFFSET, 7);
}

ClockFrameBuffers::~ClockFrameBuffers() {
  delete secondBuffer;
  delete minuteBuffer;
  delete hourBuffer;
  delete pendulumBuffer;
  delete faceInnerRingBuffer;
  delete faceOuterRingBuffer;
  delete displayLeftBuffer;
  delete displayRightBuffer;
}

void ClockFrameBuffers::updateFade() {
  secondBuffer->updateFade();
  minuteBuffer->updateFade();
  hourBuffer->updateFade();
  pendulumBuffer->updateFade();
  faceInnerRingBuffer->updateFade();
  faceOuterRingBuffer->updateFade();
  displayLeftBuffer->updateFade();
  displayRightBuffer->updateFade();
}

void ClockFrameBuffers::accelerateFadeToEnd() {
  secondBuffer->accelerateFadeToEnd();
  minuteBuffer->accelerateFadeToEnd();
  hourBuffer->accelerateFadeToEnd();
  pendulumBuffer->accelerateFadeToEnd();
  faceInnerRingBuffer->accelerateFadeToEnd();
  faceOuterRingBuffer->accelerateFadeToEnd();
  displayLeftBuffer->accelerateFadeToEnd();
  displayRightBuffer->accelerateFadeToEnd();
}
