#include "Arduino.h"
#include "ClockOptions.h"
#include "ClockMenu.h"


/*
 * Menu layout for quick understanding:
 * 
 * Main menu:
 *   "tZ" = Timezone
 *   "dS" = Daylight Saving Time
 *   "FE" = Face effects (Clock face ring effects)
 *   "Fd" = Fade effects
 *   "br" = Brightness
 *   "nb" = Night brightness (multiplied by brightness)
 *   "dY" = Display mode
 *   "UT" = Utilities
 *   
 * Timezone ("Tz") menu:
 *   Hexadecimal hour offset from GMT, ranging from "-C" (-12) to " E" (+14)
 * 
 * Daylight Saving Time ("dS") menu:
 *   Whether DST is in effect ("n" or "Y")
 * 
 * Face effects ("FE") menu:
 *   "on" = Face effects are turned on (fading between inner and outer at sunrise/sunset)
 *   "ou" = Face effects are turned off, always light outer clock face ring
 *   "in" = Face effects are turned off, always light inner clock face ring
 *   "bo" = Face effects are turned off, always light both clock face rings
 * 
 * Fade effects ("Fd") menu:
 *   Whether fade effects (persistence of vision of LEDs, sunset/sunrise fade) are enabled ("n" or "Y")
 * 
 * Brightness ("br") menu:
 *   Clock LED overall brightness, from "1" (10%) to "10" (100%)
 * 
 * Night brightness ("nb") menu:
 *   Clock LED brightness at night (between 6 PM and 6 AM), from "1" (10% of normal brightness) to "10" (100% of normal brightness)
 *   
 * Display mode ("dY") menu:
 *   "An" = Analog display mode (classic analog clock face)
 *   "bn" = Binary display mode (H/M/S are displayed in binary, using 1/6, 1/6, and 1/4 of the clock face, respectively, in clockwise direction, starting with the LSb)
 * 
 * Utilities ("UT") menu:
 *   "RS" = Reset time using GPS
 *   "L1" = LED test 1 (light all LEDs at full intensity)
 *   "L2" = LED test 2 (light each LED at full intensity in sequence)
 */

/*
 *  This is ugly, but please bear with me. 
 *  These strings are pairs of ASCII characters to be displayed on the 7-segment displays. 
 *  The length of these strings must be twice the length of each sub-menu (i.e. one character for each digit for each menu item). 
 *  Indices (MSNybble):                                      0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1
 *  Indices (LSNybble):                                      0 1 2 3 4 5 6 7 8 9 A B C D E F 0 1 2 3 4 5 6 7 8 9 A
 */
const PROGMEM char CLOCK_SUBMENU_TEXT[]                   = "TZdSFEFdbrnbdYUT";
const PROGMEM char CLOCK_SUBMENU_ITEM_TEXT_TIMEZONE[]     = "-C-b-A-9-8-7-6-5-4-3-2-1 0 1 2 3 4 5 6 7 8 9 A b C d E";
const PROGMEM char CLOCK_SUBMENU_ITEM_TEXT_BOOLEAN[]      = " n Y";
const PROGMEM char CLOCK_SUBMENU_ITEM_TEXT_FACE_EFFECTS[] = "onouinbo";
const PROGMEM char CLOCK_SUBMENU_ITEM_TEXT_DECIMAL[]      = " 1 2 3 4 5 6 7 8 910";
const PROGMEM char CLOCK_SUBMENU_ITEM_TEXT_DISPLAY_MODE[] = "Anbn";
const PROGMEM char CLOCK_SUBMENU_ITEM_TEXT_UTILITIES[]    = "RSL1L2";

constexpr uint8_t CLOCK_SUBMENU_COUNT = strlen(CLOCK_SUBMENU_TEXT) / 2;
constexpr uint8_t CLOCK_SUBMENU_LENGTH[CLOCK_SUBMENU_COUNT] = {
  strlen(CLOCK_SUBMENU_ITEM_TEXT_TIMEZONE) / 2,     // Timezone
  strlen(CLOCK_SUBMENU_ITEM_TEXT_BOOLEAN) / 2,      // DST
  strlen(CLOCK_SUBMENU_ITEM_TEXT_FACE_EFFECTS) / 2, // Face Effects
  strlen(CLOCK_SUBMENU_ITEM_TEXT_BOOLEAN) / 2,      // Fade Effects
  strlen(CLOCK_SUBMENU_ITEM_TEXT_DECIMAL) / 2,      // Brightness
  strlen(CLOCK_SUBMENU_ITEM_TEXT_DECIMAL) / 2,      // Night Brightness
  strlen(CLOCK_SUBMENU_ITEM_TEXT_DISPLAY_MODE) / 2, // Display mode
  strlen(CLOCK_SUBMENU_ITEM_TEXT_UTILITIES) / 2     // Utilities
};

const char * const CLOCK_SUBMENU_ITEM_TEXT[CLOCK_SUBMENU_COUNT] = {
  CLOCK_SUBMENU_ITEM_TEXT_TIMEZONE,
  CLOCK_SUBMENU_ITEM_TEXT_BOOLEAN,
  CLOCK_SUBMENU_ITEM_TEXT_FACE_EFFECTS,
  CLOCK_SUBMENU_ITEM_TEXT_BOOLEAN,
  CLOCK_SUBMENU_ITEM_TEXT_DECIMAL,
  CLOCK_SUBMENU_ITEM_TEXT_DECIMAL,
  CLOCK_SUBMENU_ITEM_TEXT_DISPLAY_MODE,
  CLOCK_SUBMENU_ITEM_TEXT_UTILITIES
};


ClockMenu::ClockMenu(ClockOptions &options, uint8_t selectButtonMask, uint8_t enterButtonMask, uint32_t menuTimeoutMilliseconds, uint32_t backButtonLongPressLength) : options(options) {
  this->selectButtonMask = selectButtonMask;
  this->enterButtonMask = enterButtonMask;
  
  this->menuTimeoutMilliseconds = menuTimeoutMilliseconds;
  this->backButtonLongPressLength = backButtonLongPressLength;

  lastSelectButtonPressed = false;
  lastEnterButtonPressed = false;
  currentSelectButtonPressed = false;
  currentEnterButtonPressed = false;
  longPressingSelect = false;
  lastButtonCheckMillis = 0;
  selectButtonDownMillis = 0;
  timeoutCounter = 0;

  currentMainMenuIndex = 0;
  currentSubMenuIndex = 0;

  menuTextBuffer[0] = menuTextBuffer[1] = ' ';
  menuTextBuffer[2] = 0;
}

void ClockMenu::begin() {
  DDRB &= ~(selectButtonMask | enterButtonMask);
  PORTB |= selectButtonMask | enterButtonMask;

  lastSelectButtonPressed = false;
  lastEnterButtonPressed = false;
  currentSelectButtonPressed = false;
  currentEnterButtonPressed = false;
  lastButtonCheckMillis = 0;
}

void ClockMenu::update() {
  // Update button states
  readButtons();

  if (timeoutCounter > 0) {
    if (currentSelectButtonPressed && !lastSelectButtonPressed) {
      // If the select button was just pressed, advance the menu state
      handleSelectButtonPress();
    } else if (currentEnterButtonPressed && !lastEnterButtonPressed) {
      // If the enter button was just pressed, enter the submenu or change the config option
      handleEnterButtonPress();
    }
  } else {
    // Kill the menu if it's been too long since the last button press
    exitMenu();
  }
}

bool ClockMenu::isOpen() const {
  return currentMainMenuIndex > 0;
}

const char *ClockMenu::getMenuText() const {
  return menuTextBuffer;
}


void ClockMenu::readButtons() {
  lastSelectButtonPressed = currentSelectButtonPressed;
  lastEnterButtonPressed = currentEnterButtonPressed;
  
  uint32_t currentMillis = millis();
  uint32_t intervalMillis = currentMillis - lastButtonCheckMillis;
  if (intervalMillis >= DEBOUNCE_INTERVAL) {
    currentSelectButtonPressed = (PINB & selectButtonMask) == 0;
    currentEnterButtonPressed = (PINB & enterButtonMask) == 0;
    lastButtonCheckMillis = currentMillis;

    // Check for long press to back out of the menu
    longPressingSelect = longPressingSelect && currentSelectButtonPressed;
    if (currentSelectButtonPressed && !lastSelectButtonPressed) {
      selectButtonDownMillis = currentMillis;
      longPressingSelect = true;
    } else if (longPressingSelect && (currentMillis - selectButtonDownMillis >= backButtonLongPressLength)) {
      if (currentSubMenuIndex > 0) {
        currentSubMenuIndex = 0;
        timeoutCounter = menuTimeoutMilliseconds;
      } else {
        exitMenu();
      }

      // Refresh the menu display text
      refreshMenuTextBuffer();
      
      longPressingSelect = false;
    }

    // Check for timeout
    if ((currentSelectButtonPressed != lastSelectButtonPressed) || (currentEnterButtonPressed != lastEnterButtonPressed)) {
      timeoutCounter = menuTimeoutMilliseconds;
    } else {
      timeoutCounter -= min(timeoutCounter, intervalMillis);
    }
  }
}

void ClockMenu::handleSelectButtonPress() {
  if (options.getCurrentUtilityMode() != UTILITY_MODE_NONE) {
    // If we're trapped inside of any utility mode, a menu button press will end it
    options.setCurrentUtilityMode(UTILITY_MODE_NONE);
  } else if (currentMainMenuIndex == 0) {
    // If the menu is not open, open to the first main menu option
    currentMainMenuIndex = 1;
    currentSubMenuIndex = 0;
  } else if (currentSubMenuIndex == 0) {
    // If the menu is already open, but no sub-menu is open, advance to the next sub-menu
    ++currentMainMenuIndex;
    if (currentMainMenuIndex > CLOCK_SUBMENU_COUNT) {
      currentMainMenuIndex = 1;
    }
  } else {
    // If a sub-menu is open, advance to the next item in the sub-menu
    ++currentSubMenuIndex;
    if (currentSubMenuIndex > CLOCK_SUBMENU_LENGTH[currentMainMenuIndex - 1]) {
      currentSubMenuIndex = 1;
    }
  }

  // Refresh the menu display text
  refreshMenuTextBuffer();
}

void ClockMenu::handleEnterButtonPress() {
  if (options.getCurrentUtilityMode() != UTILITY_MODE_NONE) {
    // If we're trapped inside of any utility mode, a menu button press will end it
    options.setCurrentUtilityMode(UTILITY_MODE_NONE);
  } else if (currentMainMenuIndex == 0) {
    // If the menu is not open, open to the first main menu option
    currentMainMenuIndex = 1;
    currentSubMenuIndex = 0;
  } else if (currentSubMenuIndex == 0) {
    // If the main menu is already open, enter the selected sub-menu at the correct option index
    switch(currentMainMenuIndex) {
      case 1: // Timezone
        currentSubMenuIndex = static_cast<uint8_t>(options.getTimezone() + 13);
        break;
      case 2: // DST
        currentSubMenuIndex = options.getDST() ? 2 : 1;
        break;
      case 3: // Face effects
        currentSubMenuIndex = options.getFaceEffects() + 1;
        break;
      case 4: // Fade effects
        currentSubMenuIndex = options.getFadeEffectsEnabled() ? 2 : 1;
        break;
      case 5: // Brightness
        currentSubMenuIndex = mapBrightnessOption(options.getDaytimeBrightness(), 255, 10);
        break;
      case 6: // Night brightness
        currentSubMenuIndex = mapBrightnessOption(options.getNightBrightness(), 255, 10);
        break;
      case 7: // Display mode
        currentSubMenuIndex = options.getDisplayMode() + 1;
        break;
      case 8: // Utilities
        currentSubMenuIndex = 1;
      default: // Invalid option
        break;
    }
  } else {
    // If the sub-menu is already open, set a config option
    switch(currentMainMenuIndex) {
      case 1: // Timezone
        options.setTimezone(static_cast<int8_t>(currentSubMenuIndex) - static_cast<int8_t>(13));
        break;
      case 2: // DST
        options.setDST(currentSubMenuIndex == 2);
        break;
      case 3: // Face effects
        options.setFaceEffects(currentSubMenuIndex - 1);
        break;
      case 4: // Fade effects
        options.setFadeEffectsEnabled(currentSubMenuIndex == 2);
        break;
      case 5: // Brightness
        options.setDaytimeBrightness(mapBrightnessOption(currentSubMenuIndex, 10, 255));
        break;
      case 6: // Night brightness
        options.setNightBrightness(mapBrightnessOption(currentSubMenuIndex, 10, 255));
        break;
      case 7: // Display mode
        options.setDisplayMode(currentSubMenuIndex - 1);
        break;
      case 8: // Utilities
        options.setCurrentUtilityMode(currentSubMenuIndex);
        break;
      default: // Invalid option
        break;
    }

    // Go back to the main menu
    currentSubMenuIndex = 0;
  }

  // Refresh the menu display text
  refreshMenuTextBuffer();
}

void ClockMenu::refreshMenuTextBuffer() {
  if (currentMainMenuIndex > 0) {
    const char *source;
    uint8_t offset;
    if (currentSubMenuIndex == 0) {
      // Inside the main menu
      source = CLOCK_SUBMENU_TEXT;
      offset = (currentMainMenuIndex - 1) << 1;
    } else {
      // Inside a sub-menu
      source = CLOCK_SUBMENU_ITEM_TEXT[currentMainMenuIndex - 1];
      offset = (currentSubMenuIndex - 1) << 1;
    }
    menuTextBuffer[0] = pgm_read_byte(source + offset);
    menuTextBuffer[1] = pgm_read_byte(source + offset + 1);
  } else {
    // No menu option selected (blank the display)
    menuTextBuffer[0] = menuTextBuffer[1] = ' ';
  }
}

void ClockMenu::exitMenu() {
  currentMainMenuIndex = 0;
  options.saveOptions();
}

uint8_t ClockMenu::mapBrightnessOption(uint8_t option, uint8_t fromRange, uint8_t toRange) {
  uint16_t numerator = static_cast<uint16_t>(option) * static_cast<uint16_t>(toRange);
  uint16_t result = numerator / static_cast<uint16_t>(fromRange);
  if ((numerator % static_cast<uint16_t>(fromRange)) >= (fromRange >> 1)) {
    ++result;
  }
  return static_cast<uint8_t>(result);
}
