arduino-rc-nav-led
==================

RC aircraft navigation, anti-collision, and strobe lights using an Arduino or ATTiny85 microcontroller. See a demo of the code in action at https://www.youtube.com/watch?v=tBnNrIOuIKM

Current Features
----------------
* Landing light controlled via servo channel (can use a Y-harness off flaps or gear channel)
* Realistic dual anti-collision beacon, fades in/out to simulate rotating
* Realistic Double-blink strobe

Default pinout for the ATTiny85
-------------------------------
The code is pre-configured for use with the ATTiny85 microprocessor. It should also run on any arduino board, but the pin numbers will likely need to be modified.

* Pin 1 - Reset, not used
* Pin 2 - Landing light output
* Pin 3 - Strobe light output
* Pin 4 - Ground
* Pin 5 - Anti-collision 1 output
* Pin 6 - Anti-collision 2 output
* Pin 7 - Servo signal PWM input for landing light
* Pin 8 - VCC (2.7 to 5.5VDC)

If you are using standard low-power LEDs which draw less than 40mA on each output, you can drive them directly from the ATTiny85. If you want to drive high power LEDs, you will need to use transistors.
