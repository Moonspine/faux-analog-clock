#include "Arduino.h"
#include "FrameBufferView.h"

FrameBufferView::FrameBufferView(uint8_t *frameBuffer, uint8_t count) {
  this->frameBuffer = frameBuffer;
  this->count = count;

  targetFadeValue = 0;
  fadeRate = 0;
  fadeTickDelay = 0;

  remainingFadeTickDelay = 0;
  isFadeActive = false;
}

void FrameBufferView::setValue(uint8_t index, uint8_t value) {
  if (index < count) {
    frameBuffer[index] = value;
    isFadeActive = fadeRate > 0;
  }
}

void FrameBufferView::setAllValues(uint8_t value) {
  memset(frameBuffer, value, count);
  isFadeActive = fadeRate > 0;
}

void FrameBufferView::initializeFade(uint8_t fadeRate, uint16_t fadeTickDelay, uint8_t targetFadeValue) {
  this->fadeRate = fadeRate;
  this->fadeTickDelay = fadeTickDelay;
  this->targetFadeValue = targetFadeValue;
  
  remainingFadeTickDelay = fadeTickDelay;
  isFadeActive = fadeRate > 0;
}

void FrameBufferView::initializeFade(uint8_t fadeRate, uint16_t fadeTickDelay) {
  this->fadeRate = fadeRate;
  this->fadeTickDelay = fadeTickDelay;
  
  remainingFadeTickDelay = fadeTickDelay;
  isFadeActive = fadeRate > 0;
}

void FrameBufferView::setFadeTarget(uint8_t targetFadeValue) {
  this->targetFadeValue = targetFadeValue;
  isFadeActive = fadeRate > 0;
}

void FrameBufferView::updateFade() {
  if (isFadeActive) {
    if (remainingFadeTickDelay > 0) {
      // Update tick delay
      --remainingFadeTickDelay;
    } else {
      // Fade buffer
      isFadeActive = false;
      uint8_t *currentBuffer = frameBuffer;
      for (uint8_t i = 0; i < count; ++i) {
        uint8_t value = *currentBuffer;
        if (value != targetFadeValue) {
          if (value > targetFadeValue) {
            // If value is greater than the target value, decrease it
            *currentBuffer -= min(fadeRate, value - targetFadeValue);
          } else {
            // If value is less than the target value, increase it
            *currentBuffer += min(fadeRate, targetFadeValue - value);
          }
          isFadeActive = true;
        }
        
        ++currentBuffer;
      }

      // Set delay for the next fade tick
      remainingFadeTickDelay = fadeTickDelay;
    }
  }
}

void FrameBufferView::accelerateFadeToEnd() {
  if (isFadeActive) {
    memset(frameBuffer, targetFadeValue, count);
    isFadeActive = false;
  }
}
