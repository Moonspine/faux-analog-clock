#include "Arduino.h"
#include "FrameBufferView.h"

FrameBufferView::FrameBufferView(uint8_t *frameBuffer, uint8_t count) {
  this->frameBuffer = frameBuffer;
  this->count = count;

  microsecondsPerFadeTick = 0;
  targetFadeValue = 0;

  isFadeActive = false;
  lastFadeActive = false;
  lastFadeTimestamp = 0;
}

void FrameBufferView::setValue(uint8_t index, uint8_t value) {
  if (index < count) {
    frameBuffer[index] = value;
    isFadeActive = microsecondsPerFadeTick > 0;
  }
}

void FrameBufferView::setAllValues(uint8_t value) {
  memset(frameBuffer, value, count);
  isFadeActive = microsecondsPerFadeTick > 0;
}

void FrameBufferView::setValuesBinaryDisplay(uint8_t bitValue, uint8_t bitCount, uint8_t valuesPerBit, bool setZeroes, uint8_t intensity) {
  // Sanity checks
  uint8_t realBitCount = min(max(bitCount, 1), 8);
  uint8_t realValuesPerBit = min(max(valuesPerBit, 1), count / realBitCount);

  // Set values
  uint8_t remainingValue = bitValue;
  uint8_t *target = frameBuffer;
  for (uint8_t i = 0; i < realBitCount; ++i) {
    bool isOne = (remainingValue & 1) > 0;

    if (isOne || setZeroes) {
      memset(target, isOne ? intensity : 0, realValuesPerBit);
    }

    target += realValuesPerBit;
    remainingValue >>= 1;
  }
  
  isFadeActive = microsecondsPerFadeTick > 0;
}

void FrameBufferView::initializeFade(uint32_t microsecondsPerFadeTick, uint8_t targetFadeValue) {
  this->microsecondsPerFadeTick = microsecondsPerFadeTick;
  this->targetFadeValue = targetFadeValue;
  
  isFadeActive = microsecondsPerFadeTick > 0;
}

void FrameBufferView::initializeFade(uint32_t microsecondsPerFadeTick) {
  this->microsecondsPerFadeTick = microsecondsPerFadeTick;
  
  isFadeActive = microsecondsPerFadeTick > 0;
}

void FrameBufferView::setFadeTarget(uint8_t targetFadeValue) {
  this->targetFadeValue = targetFadeValue;
  isFadeActive = microsecondsPerFadeTick > 0;
}

void FrameBufferView::updateFade() {
  if (isFadeActive) {
    uint32_t timestamp = micros();
    bool firstFadeTick = !lastFadeActive;

    if (firstFadeTick) {
      // If first fade tick, just grab a timestamp
      firstFadeTick = false;
      lastFadeTimestamp = timestamp;
    } else {
      // Compute fade rate and upadte timing
      uint32_t fadeRateLong = (timestamp - lastFadeTimestamp) / microsecondsPerFadeTick;
      lastFadeTimestamp += fadeRateLong * microsecondsPerFadeTick;

      uint8_t fadeRate = static_cast<uint8_t>(min(fadeRateLong, 255));
      if (fadeRate > 0) {
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
      }
    }
  }
  lastFadeActive = isFadeActive;
}

void FrameBufferView::accelerateFadeToEnd() {
  if (isFadeActive) {
    memset(frameBuffer, targetFadeValue, count);
    isFadeActive = false;
  }
}
