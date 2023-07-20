# Faux Analog Clock
An analog clock composed entirely of LEDs

The build should be fairly self-explanatory. A KiCad 6 schematic and board layout are included in `Board\`  
Additionally, `Board\Faux Analog Clock Gerber.zip` is a ready-to-go Gerber plot of the board which can be sent directly to the PCB manufacturer of your choice.  
LED resistor values are mere suggetions, and you should experiment on a breadboard to see what works well with your chosen LEDs.


# Using the menu

Press "Select" or "Enter" to enter the menu.  
Subsequent presses of "Select" will advance to different menu options.  
Pressing "Enter" will choose a menu option (either changing a setting or opening a submenu.)  
Pressing and holding "Select" for the amount of time defined by MENU_BACK_BUTTON_LONG_PRESS_MS will return to the previous menu (or close the menu if at the top menu.)  
If no buttons are pressed for MENU_TIMEOUT_MS milliseconds, the menu will close on its own.


## Main menu options
- tZ (72) = Timezone
- dS (d5) = Daylight Saving Time
- FE (FE) = Face effects (Clock face ring effects)
- Fd (Fd) = Fade effects
- br (br) = Brightness
- nb (nb) = Night brightness (multiplied by brightness)
- UT (U7) = Utilities


## Timezone menu

In this menu, you can select the timezone offset of the clock, in whle hour increments.  
These values are displayed in hexadecimal (sorry, 7-segment displays) ranging from "-C" (-12 hours) to "E" (+14 hours)


## Daylight Saving Time menu

In this menu, you can set whether DST is in effect ("Y") or not ("n")


## Face Effects menu

This menu controls the face edge lighting.
- "on" = Face effects are turned on (fading between inner and outer at sunrise/sunset)
- "ou" = Face effects are turned off, always light outer clock face ring
- "in" = Face effects are turned off, always light inner clock face ring
- "bo" = Face effects are turned off, always light both clock face rings


## Fade Effects menu

This menu turns the LED intensity fade effects on ("Y") or off ("n")


## Brightness menu

This menu controls overall clock LED brightness from 1 (dimmest) to 10 (brightest).


## Night Brightness menu

This menu controls overall clock LED brightness at night (from 6 PM to 6 AM) from 1 (dimmest) to 10 (brightest).  
This value is multiplied by the overall clock brightness, making it effectively a scalar for clock brightness that's only applied at night.


## Utilities menu

This menu includes some useful utilities for testing your clock build.
- "RS" ("r5") = Reset the current time using the GPS
- "L1"        = Run LED test 1 (light all LEDs at max intensity) (press any button to end)
- "L2"        = Run LED test 2 (light LEDs at max intensity sequentially) (press any button to end)



# Customizing the clock

## Faux_Analog_Clock.ino

At the top of `Firmware/Faux_Analog_Clock/Faux_Analog_Clock.ino`, you can find many constants defined which alter the behavior of the clock.

You can change the fade animation rate by modifying variables in the "Animation timing variables" section.
- CLOCK_ANIM_*_FADE_RATE       = The change in brightness on each fade tick.
- CLOCK_ANIM_*_FADE_TICK_DELAY = The number of rendered frames which constitutes one fade tick.

Menu behavior can be configured as well:
- MENU_TIMEOUT_MS                = The number of milliseconds until the menu auto-closes after the last button press.
- MENU_BACK_BUTTON_LONG_PRESS_MS = The number of milliseconds the user must hold the "Select" button to return to the previous menu.

You can also change how often the GPS is used to set the RTC:
- TIMEKEEPER_GPS_TIME_SET_INTERVAL_SECONDS = Seconds in between time resets.

## Timekeeper.h

Finally, in `Firmware/Faux_Analog_Clock/Timekeeper.h`, commenting out the following line will cause the timekeeper to use an internal millis()-based RTC rather than external RTC hardware:
```
#define USE_HARDWARE_RTC 1
```
