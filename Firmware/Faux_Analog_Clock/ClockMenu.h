#ifndef CLOCK_MENU_H
#define CLOCK_MENU_H

#include "Arduino.h"
#include "ClockOptions.h"

const uint32_t DEBOUNCE_INTERVAL = 50;

class ClockMenu {
public:
  /**
   * Creates a new clock menu object using the given PORTB pins for its input buttons
   * 
   * @param options The clock options to configure with this menu
   * @param selectButtonMask The bitmask of the PORTB "select" button
   * @param enterButtonMask The bitmask of the PORTB "enter" button
   * @param menuTimeoutMilliseconds The number of milliseconds between the last button press and the automatic closing of the menu
   * @param backButtonLongPressLength The amount of time that the user must hold the "Select" button to got back one menu
   */
  ClockMenu(ClockOptions &options, uint8_t selectButtonMask, uint8_t enterButtonMask, uint32_t menuTimeoutMilliseconds, uint32_t backButtonLongPressLength);

  /**
   * Initialized the I/O pins
   */
  void begin();

  /**
   * Updates the menu, reading button presses and setting options accordingly
   */
  void update();

  /**
   * Determines whether the menu is currently open
   */
  bool isOpen() const;

  /**
   * Gets the display text of the current menu item
   */
  const char *getMenuText() const;
  
private:
  ClockOptions &options;
  
  uint8_t selectButtonMask;
  uint8_t enterButtonMask;

  uint32_t menuTimeoutMilliseconds;
  uint32_t backButtonLongPressLength;

  bool lastSelectButtonPressed;
  bool lastEnterButtonPressed;
  bool currentSelectButtonPressed;
  bool currentEnterButtonPressed;
  bool longPressingSelect;
  uint32_t lastButtonCheckMillis;
  uint32_t selectButtonDownMillis;
  uint32_t timeoutCounter;

  uint8_t currentMainMenuIndex;
  uint8_t currentSubMenuIndex;

  char menuTextBuffer[3];

  /**
   * Reads the state of the buttons
   */
  void readButtons();

  /**
   * Handles a select button press
   */
  void handleSelectButtonPress();

  /**
   * Handles an enter button press
   */
  void handleEnterButtonPress();

  /**
   * Refreshes the menu text buffer with the text for the current menu option
   */
  void refreshMenuTextBuffer();

  /**
   * Exits the menu, saving configuration data
   */
  void exitMenu();

  /**
   * Scales a brightness option from the 0..fromRange range to the 0..toRange range
   * 
   * @param option The option to scale
   * @param fromRange The range of the option
   * @param toRange The range to scale it into
   */
  uint8_t mapBrightnessOption(uint8_t option, uint8_t fromRange, uint8_t toRange);
};

#endif
