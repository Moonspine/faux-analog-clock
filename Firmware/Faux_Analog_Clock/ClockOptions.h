#ifndef CLOCK_OPTIONS_H
#define CLOCK_OPTIONS_H

#include "Arduino.h"

/**
 * All face effects are applied (fading colors during sunrise, dimmed in the evening)
 */
const uint8_t FACE_EFFECTS_ON = 0;

/**
 * No face effects are applied (light the outer ring at full intensity)
 */
const uint8_t FACE_EFFECTS_OUTER = 1;

/**
 * No face effects are applied (light the inner ring at full intensity)
 */
const uint8_t FACE_EFFECTS_INNER = 2;

/**
 * No face effects are applied (light both rings at full intensity)
 */
const uint8_t FACE_EFFECTS_BOTH = 3;

/**
 * Various utility modes used by the menu to test various clock features
 */
const uint8_t UTILITY_MODE_NONE = 0;
const uint8_t UTILITY_MODE_RESET_TIME = 1;
const uint8_t UTILITY_MODE_LED_TEST_1 = 2;
const uint8_t UTILITY_MODE_LED_TEST_2 = 3;

/**
 * Display mode values
 */
const uint8_t CLOCK_DISPLAY_MODE_ANALOG = 0;
const uint8_t CLOCK_DISPLAY_MODE_BINARY = 1;


/**
 * Class containing clock options, as well as handling saving/loading of options from EEPROM
 */
class ClockOptions {
public:
  /**
   * Initializes a clock option object, loading the last saved options from EEPROM
   */
  ClockOptions();

  /**
   * Sets the current timezone as an offset from GMT
   */
  void setTimezone(int8_t timezone);

  /**
   * Sets whether we are in DST mode
   */
  void setDST(bool dst);

  /**
   * Sets which face effects are applied (see FACE_EFFECTS_* for more info)
   */
  void setFaceEffects(uint8_t mode);

  /**
   * Sets whether or not fade effects are enabled
   */
  void setFadeEffectsEnabled(bool enabled);

  /**
   * Sets the brightness of the clock LEDs during the day (6 AM to 6 PM)
   */
  void setDaytimeBrightness(uint8_t value);

  /**
   * Sets the brightness of the clock LEDs during the night (6 PM to 6 AM)
   */
  void setNightBrightness(uint8_t value);

  /**
   * Sets the clock display mode (see CLOCK_DISPLAY_MODE_* for valid values)
   */
  void setDisplayMode(uint8_t mode);

  /**
   * Sets the current utility mode (see UTILITY_MODE_* for more info)
   */
  void setCurrentUtilityMode(uint8_t mode);



  /**
   * Gets the current timezone as an offset from GMT
   */
  int8_t getTimezone() const;

  /**
   * Gets whether or not the clock is in DST mode
   */
  bool getDST() const;

  /**
   * Gets which face effects are applied (see FACE_EFFECTS_* for more info)
   */
  uint8_t getFaceEffects() const;

  /**
   * Gets whether or not fade effects are enabled
   */
  bool getFadeEffectsEnabled() const;

  /**
   * Gets the brightness of the clock LEDs during the day (6 AM to 6 PM)
   */
  uint8_t getDaytimeBrightness() const;

  /**
   * Gets the brightness of the clock LEDs during the night (6 PM to 6 AM)
   */
  uint8_t getNightBrightness() const;
  
  /**
   * Gets the current utility mode (see UTILITY_MODE_* for more info)
   */
  uint8_t getCurrentUtilityMode() const;

  /**
   * Gets the premultiplied night brightness value (day brightness * night brightness)
   */
  uint8_t getPremultipliedNightBrightness() const;

  /**
   * Gets the clock display mode (see CLOCK_DISPLAY_MODE_* for valid values)
   */
  uint8_t getDisplayMode() const;

  /**
   * Returns true if any options have changed since the last time this method was called
   */
  bool getOptionsChanged();


  /**
   * Saves any changed clock options to EEPROM
   */
  void saveOptions();
  

private:
  int8_t timezone;
  bool dst;
  uint8_t faceEffects;
  bool fadeEffectsEnabled;
  uint8_t daytimeBrightness;
  uint8_t nightBrightness;
  uint8_t displayMode;

  uint8_t premultipliedNightBrightness;
  bool changed;
  
  uint8_t currentUtilityMode;


  /**
   * Loads clock options from EEPROM
   */
  void loadOptions();

  /**
   * Updates the premultiplied night brightness value
   */
  void updatePremultipliedNightBrightness();
};

#endif
