#include "ClockDisplay.h"
#include "Timekeeper.h"
#include "ClockFace.h"
#include "ClockOptions.h"
#include "SevenSegment.h"
#include "ClockMenu.h"
#include "ClockFrameBuffers.h"
#include "ClockDisplayMode.h"

/**
 * Faux Analog Clock
 * 
 * An analog clock composed entirely of LEDs.
 * See the KiCad schematic/board design for more info.
 * 
 * Required libraries:
 *  Adafruit RTCLib 2.1.1
 *  Adafruit BusIO 1.14.1
 *  Adafruit GPS 1.7.2
 * 
 * Pinout:
 *  RTC SDA            = PC4
 *  RTC SCL            = PC5
 *  GPS TX             = PC2
 *  GPS RX             = PC3
 *  Charlieplex 0..7   = PORTD 0..7
 *  Charlieplex 8..11  = PORTB 0..3
 *  Charlieplex 12..13 = PORTC 0..1
 *  Select Button      = PB4
 *  Enter Button       = PB5
 */

/*
 * Animation timing variables (tweak these to tweak the clock fade animations)
 */
const uint32_t CLOCK_ANIM_SECONDS_FADE_TIME = 2000;
const uint32_t CLOCK_ANIM_MINUTES_FADE_TIME = 2000;
const uint32_t CLOCK_ANIM_HOURS_FADE_TIME = 4000;
const uint32_t CLOCK_ANIM_PENDULUM_FADE_TIME = 2000;
const uint32_t CLOCK_ANIM_RING_FADE_TIME = 1500000;

/*
 * Menu configuration
 */
const uint8_t MENU_SELECT_BUTTON_MASK = 0b00010000;
const uint8_t MENU_ENTER_BUTTON_MASK = 0b00100000;
const uint32_t MENU_TIMEOUT_MS = 10000;
const uint32_t MENU_BACK_BUTTON_LONG_PRESS_MS = 1000;

/*
 * Timekeeper configuration
 */
const uint32_t TIMEKEEPER_GPS_TIME_SET_INTERVAL_SECONDS = 86400;


/*
 * Various classes which are integral to clock operation
 */
ClockDisplay clockDisplay;
ClockFrameBuffers clockFrameBuffers(clockDisplay);
Timekeeper timekeeper(A2, A3, TIMEKEEPER_GPS_TIME_SET_INTERVAL_SECONDS);

ClockOptions options;
ClockMenu menu(options, MENU_SELECT_BUTTON_MASK, MENU_ENTER_BUTTON_MASK, MENU_TIMEOUT_MS, MENU_BACK_BUTTON_LONG_PRESS_MS);

ClockDisplayMode *displayMode = new AnalogClockDisplayMode();


// Clock set animation vars
uint8_t clockSetAnimationValue = 0;
int8_t clockSetAnimationDirection = 1;


// Main initialization routine
void setup() {
  // Start the clock display
  clockDisplay.begin();

  // Uncomment this block to do an LED test (useful while building the board)
  /*
  clockDisplay.setAllLEDValues(255);
  while (true) {
    clockDisplay.display();
  }
  */

  // Do LED fade test
  runLEDFadeTest();

  // Initialize timing buffers
  clockFrameBuffers.getSecondBuffer()->initializeFade(CLOCK_ANIM_SECONDS_FADE_TIME, 0);
  clockFrameBuffers.getMinuteBuffer()->initializeFade(CLOCK_ANIM_MINUTES_FADE_TIME, 0);
  clockFrameBuffers.getHourBuffer()->initializeFade(CLOCK_ANIM_HOURS_FADE_TIME, 0);
  clockFrameBuffers.getPendulumBuffer()->initializeFade(CLOCK_ANIM_PENDULUM_FADE_TIME, 0);

  // Initialize face ring buffers
  clockFrameBuffers.getFaceInnerRingBuffer()->initializeFade(CLOCK_ANIM_RING_FADE_TIME, 0);
  clockFrameBuffers.getFaceInnerRingBuffer()->setAllValues(0);
  clockFrameBuffers.getFaceOuterRingBuffer()->initializeFade(CLOCK_ANIM_RING_FADE_TIME, options.getDaytimeBrightness());
  clockFrameBuffers.getFaceOuterRingBuffer()->setAllValues(options.getDaytimeBrightness());

  // Start the timekeeper
  timekeeper.begin();
  timekeeper.setTimeZone(options.getTimezone(), options.getDST());

  // Start the menu
  menu.begin();
}

// Main loop
void loop() {
  bool wasTimeSetPendingLastIteration = timekeeper.isTimeSetPending();
  timekeeper.update();

  if (timekeeper.isTimeValid()) {
    // Update time
    const DateTime &now = timekeeper.getTime();
    bool isNight = now.hour() < 6 || now.hour() > 18;
    
    // Update menu and options
    menu.update();
    uint8_t brightness = isNight ? options.getPremultipliedNightBrightness() : options.getDaytimeBrightness();
    if (options.getOptionsChanged()) {
      updateOptions(brightness);
    }
    
    // Update fade
    if (options.getFadeEffectsEnabled()) {
      clockFrameBuffers.updateFade();
    } else {
      clockFrameBuffers.accelerateFadeToEnd();
    }

    // Update time display
    displayMode->update(clockFrameBuffers, now, timekeeper.getMilliseconds(), brightness);

    // Update clock ring animation fade targets
    updateClockRingFadeTargets(now.hour(), brightness);

    // Display menu?
    if (menu.isOpen()) {
      const char *menuText = menu.getMenuText();
      writeSevenSegmentDisplay(clockFrameBuffers.getDisplayLeftBuffer(), menuText[0], brightness);
      writeSevenSegmentDisplay(clockFrameBuffers.getDisplayRightBuffer(), menuText[1], brightness);
    } else {
      // AM/PM indicator
      if (now.isPM()) {
        writeSevenSegmentDisplay(clockFrameBuffers.getDisplayLeftBuffer(), ' ', brightness);
        writeSevenSegmentDisplay(clockFrameBuffers.getDisplayRightBuffer(), 'P', brightness);
      } else {
        writeSevenSegmentDisplay(clockFrameBuffers.getDisplayLeftBuffer(), 'A', brightness);
        writeSevenSegmentDisplay(clockFrameBuffers.getDisplayRightBuffer(), ' ', brightness);
      }
    }

    // Update the time set animation and handle its aftermath
    if (timekeeper.isTimeSetPending()) {
      // If we're actively setting the time, play the time set animation
      updateTimeSetAnimation();
    } else if (!wasTimeSetPendingLastIteration) {
      // If we previously had no time set (i.e. were playing the time set animation), snap the clock rings to whatever time they should be set for
      clockSetAnimationValue = 0;
      clockSetAnimationDirection = 1;
      updateClockFaceEffectMode(brightness);
    }
  } else {
    // Animate the clock face to show that the time is being set
    updateTimeSetAnimation();
  }

  // Display the clock LEDs
  clockDisplay.display();
}

/**
 * Updates the clock with any changed options
 * 
 * @param brightness The current brightness of the clock
 */
void updateOptions(uint8_t brightness) {
  // Set timezone
  timekeeper.setTimeZone(options.getTimezone(), options.getDST());

  // Update clock face effect mode
  updateClockFaceEffectMode(brightness);

  // Update clock display mode
  delete displayMode;
  switch (options.getDisplayMode()) {
    case CLOCK_DISPLAY_MODE_BINARY:
      displayMode = new BinaryClockDisplayMode();
      break;
    case CLOCK_DISPLAY_MODE_INVERTED_ANALOG:
      displayMode = new InvertedAnalogClockDisplayMode();
      break;
    case CLOCK_DISPLAY_MODE_FILL:
      displayMode = new FillClockDisplayMode();
      break;
    case CLOCK_DISPLAY_MODE_FILL_UNFILL:
      displayMode = new FillUnfillClockDisplayMode();
      break;
    default:
    case CLOCK_DISPLAY_MODE_ANALOG:
      displayMode = new AnalogClockDisplayMode();
      break;
  }
  displayMode->initialize(clockFrameBuffers, brightness);

  // Handle utility modes
  switch (options.getCurrentUtilityMode()) {
    case UTILITY_MODE_RESET_TIME:
      timekeeper.resetTime();
      options.setCurrentUtilityMode(UTILITY_MODE_NONE);
      break;
    case UTILITY_MODE_LED_TEST_1:
      runLedTest1();
      break;
    case UTILITY_MODE_LED_TEST_2:
      runLedTest2();
      break;
    default:
      break;
  }
}

/**
 * Updates the clock face frame buffers to reflect the current face effect mode
 * 
 * @param brightness The current brightness of the clock
 */
void updateClockFaceEffectMode(uint8_t brightness) {
  switch (options.getFaceEffects()) {
    case FACE_EFFECTS_OUTER:
      clockFrameBuffers.getFaceInnerRingBuffer()->initializeFade(0, 0);
      clockFrameBuffers.getFaceOuterRingBuffer()->initializeFade(0, 0);
      clockFrameBuffers.getFaceInnerRingBuffer()->setAllValues(0);
      clockFrameBuffers.getFaceOuterRingBuffer()->setAllValues(brightness);
      break;
    case FACE_EFFECTS_INNER:
      clockFrameBuffers.getFaceInnerRingBuffer()->initializeFade(0, 0);
      clockFrameBuffers.getFaceOuterRingBuffer()->initializeFade(0, 0);
      clockFrameBuffers.getFaceInnerRingBuffer()->setAllValues(brightness);
      clockFrameBuffers.getFaceOuterRingBuffer()->setAllValues(0);
      break;
    case FACE_EFFECTS_BOTH:
      clockFrameBuffers.getFaceInnerRingBuffer()->initializeFade(0, 0);
      clockFrameBuffers.getFaceOuterRingBuffer()->initializeFade(0, 0);
      clockFrameBuffers.getFaceInnerRingBuffer()->setAllValues(brightness);
      clockFrameBuffers.getFaceOuterRingBuffer()->setAllValues(brightness);
      break;
    case FACE_EFFECTS_ON:
    default:
      clockFrameBuffers.getFaceInnerRingBuffer()->initializeFade(CLOCK_ANIM_RING_FADE_TIME);
      clockFrameBuffers.getFaceOuterRingBuffer()->initializeFade(CLOCK_ANIM_RING_FADE_TIME);
      updateClockRingFadeTargets(timekeeper.getTime().hour(), brightness);
      clockFrameBuffers.getFaceInnerRingBuffer()->accelerateFadeToEnd();
      clockFrameBuffers.getFaceOuterRingBuffer()->accelerateFadeToEnd();
      break;
  }
}

/**
 * Updates the animation that plays while the time is being actively set
 */
void updateTimeSetAnimation() {
  clockFrameBuffers.getFaceInnerRingBuffer()->setAllValues(255 - clockSetAnimationValue);
  clockFrameBuffers.getFaceOuterRingBuffer()->setAllValues(clockSetAnimationValue);
  clockSetAnimationValue += clockSetAnimationDirection * 2;
  if (clockSetAnimationValue == 0 || clockSetAnimationValue == 250) {
    clockSetAnimationDirection = -clockSetAnimationDirection;
  }
}

/**
 * Updates the clock ring fade targets
 * 
 * @param hour The current hour
 * @param brightness The brightness
 */
inline void updateClockRingFadeTargets(uint8_t hour, uint8_t brightness) {
    clockFrameBuffers.getFaceInnerRingBuffer()->setFadeTarget(multiplyBrightness(pgm_read_byte(FACE_INNER_HOURLY_BRIGHTNESS + hour), brightness));
    clockFrameBuffers.getFaceOuterRingBuffer()->setFadeTarget(multiplyBrightness(pgm_read_byte(FACE_OUTER_HOURLY_BRIGHTNESS + hour), brightness));
}

/**
 * Multiplies the given value by the given brightness
 */
inline uint8_t multiplyBrightness(uint8_t value, uint8_t brightness) {
  return static_cast<uint8_t>(static_cast<uint16_t>(value) * static_cast<uint16_t>(brightness) / static_cast<uint16_t>(255));
}


/**
 * A sort of self-test which fades all of the LEDs in and out
 */
void runLEDFadeTest() {
  for (uint16_t i = 0; i < 256; i += 4) {
    clockDisplay.setAllLEDValues(static_cast<uint8_t>(i));
    clockDisplay.display();
  }
  clockDisplay.setAllLEDValues(255);
  for (uint16_t i = 0; i < 60; ++i) {
    clockDisplay.display();
  }
  for (uint16_t i = 0; i < 256; i += 4) {
    clockDisplay.setAllLEDValues(255 - static_cast<uint8_t>(i));
    clockDisplay.display();
  }
  clockDisplay.setAllLEDValues(0);
}

/**
 * LED test 1 - Show all LEDs at full intensity
 */
void runLedTest1() {
  clockDisplay.setAllLEDValues(255);
  while (options.getCurrentUtilityMode() != UTILITY_MODE_NONE) {
    menu.update();
    clockDisplay.display();
  }
  clockDisplay.setAllLEDValues(0);
}

/**
 * LED test 2 - Show each LED at full intensity in sequence
 */
void runLedTest2() {
  uint8_t i = 0;
  uint16_t timer = 0;
  clockDisplay.setAllLEDValues(0);
  clockDisplay.setLEDValue(i, 255);
  while (options.getCurrentUtilityMode() != UTILITY_MODE_NONE) {
    menu.update();
    clockDisplay.display();

    ++timer;
    if (timer >= 1024) {
      clockDisplay.setLEDValue(i, 0);
      i = (i + 1) % CLOCK_DISPLAY_LED_COUNT;
      clockDisplay.setLEDValue(i, 255);
      timer = 0;
    }
  }
  clockDisplay.setAllLEDValues(0);
}
