#include "Arduino.h"
#include "ClockOptions.h"
#include "EEPROM.h"

const uint8_t CONFIG_VERSION = 1;

ClockOptions::ClockOptions() {
  loadOptions();
  currentUtilityMode = 0;
  changed = true;
}

void ClockOptions::setTimezone(int8_t timezone) {
  this->timezone = timezone;
  changed = true;
}

void ClockOptions::setDST(bool dst) {
  this->dst = dst;
  changed = true;
}

void ClockOptions::setFaceEffects(uint8_t mode) {
  this->faceEffects = mode;
  changed = true;
}

void ClockOptions::setFadeEffectsEnabled(bool enabled) {
  fadeEffectsEnabled = enabled;
  changed = true;
}

void ClockOptions::setDaytimeBrightness(uint8_t value) {
  this->daytimeBrightness = value;
  updatePremultipliedNightBrightness();
  changed = true;
}

void ClockOptions::setNightBrightness(uint8_t value) {
  this->nightBrightness = value;
  updatePremultipliedNightBrightness();
  changed = true;
}

void ClockOptions::setDisplayMode(uint8_t mode) {
  this->displayMode = mode;
  changed = true;
}

void ClockOptions::setPendulumPeriod(uint8_t numSeconds) {
  this->pendulumPeriod = max(numSeconds, 1);
}

void ClockOptions::setCurrentUtilityMode(uint8_t mode) {
  this->currentUtilityMode = mode;
  changed = true;
}


int8_t ClockOptions::getTimezone() const {
  return timezone;
}

bool ClockOptions::getDST() const {
  return dst;
}

uint8_t ClockOptions::getFaceEffects() const {
  return faceEffects;
}

bool ClockOptions::getFadeEffectsEnabled() const {
  return fadeEffectsEnabled;
}

uint8_t ClockOptions::getDaytimeBrightness() const {
  return daytimeBrightness;
}

uint8_t ClockOptions::getNightBrightness() const {
  return nightBrightness;
}

uint8_t ClockOptions::getCurrentUtilityMode() const {
  return currentUtilityMode;
}

uint8_t ClockOptions::getPremultipliedNightBrightness() const {
  return premultipliedNightBrightness;
}

uint8_t ClockOptions::getDisplayMode() const {
  return displayMode;
}

uint8_t ClockOptions::getPendulumPeriod() const {
  return pendulumPeriod;
}

bool ClockOptions::getOptionsChanged() {
  bool result = changed;
  changed = false;
  return result;
}


void ClockOptions::saveOptions() {
  EEPROM.put(0, CONFIG_VERSION);
  EEPROM.put(1, timezone);
  EEPROM.put(2, dst);
  EEPROM.put(3, faceEffects);
  EEPROM.put(4, fadeEffectsEnabled);
  EEPROM.put(5, daytimeBrightness);
  EEPROM.put(6, nightBrightness);
  EEPROM.put(7, displayMode);
  EEPROM.put(8, pendulumPeriod);
}



void ClockOptions::loadOptions() {
  uint8_t version;
  EEPROM.get(0, version);
  if (version < 1 || version > CONFIG_VERSION) {
    // No valid config data present; reset to defaults
    timezone = -6;
    dst = true;
    faceEffects = FACE_EFFECTS_ON;
    fadeEffectsEnabled = true;
    daytimeBrightness = 255;
    nightBrightness = 255;
    displayMode = CLOCK_DISPLAY_MODE_ANALOG;
    pendulumPeriod = 1;
  } else {
    EEPROM.get(1, timezone);
    EEPROM.get(2, dst);
    EEPROM.get(3, faceEffects);
    EEPROM.get(4, fadeEffectsEnabled);
    EEPROM.get(5, daytimeBrightness);
    EEPROM.get(6, nightBrightness);
    EEPROM.get(7, displayMode);
    EEPROM.get(8, pendulumPeriod);
  }
  updatePremultipliedNightBrightness();
}

void ClockOptions::updatePremultipliedNightBrightness() {
  premultipliedNightBrightness = static_cast<uint8_t>(static_cast<uint16_t>(daytimeBrightness) * static_cast<uint16_t>(nightBrightness) / static_cast<uint16_t>(255));
}
