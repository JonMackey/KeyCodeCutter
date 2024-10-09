# Key Code Cutter
<p align="center">
  <img width="527" height="610" src="Key Code Cutter Front.png">
</p>

<p align="center">
  <img width="619" height="568" src="Key Code Cutter Side.png">
</p>

<p align="center">
  <img width="669" height="518" src="Key Code Cutter Top.png">
</p>

This project cuts keys for traditional round pinned locks based on a manufacturer's keyway specification and the code stamped on the original keys provided with the lockset.  The code can also be determined by measuring the depths manually.  The software supports cutting custom non-standard key depths by entering the custom depth for each pin.

The software is designed to run on my "3 Axis CNC STM32 v1.0" board.
<p align="center">
  <img width="714" height="535" src="Anotated Board.jpeg">
</p>

Note: There are several motor speed controllers that use a 5 wire switched potentiometer (2 wires on/off, 3 wires for the pot.)  The 12V~48V DC motor speed controller control circuit must be 5V in order to be compatible with the CNC board's digital 5V potentiometer.  Otherwise only the high-side switch should be used.

## TeensyStep
This project uses a modified version of Lutz Niggl's [TeensyStep](https://github.com/luni64/TeensyStep/tree/master) Copyright (c) 2017.  The changes allow for use of STM32F103 microprocessors.  My modifications implement a one-shot PWM stepper pulse and selectable timer assignments.  The original code randomly assigned timers.  The use of PWM potentially uses more timers than the original code, but offloads generating step pulses to the hardware.  Using PWM also ensures all pulses are of the same precise duration which makes viewing the timing on a data analizer easier to check for accuracy.  The modified library is included in this repository.  Note that the changes have only been tested with the LinStepAccelerator configuration.

## SdFat
This project uses an unmodified version of Bill Greiman's SdFat library. Copyright Bill Greiman 2011-2024 (currently using version 2.2.3)  This can be loaded using the Arduino IDE's library manager.

### Touchscreen Support
The touchscreen uses my XPT2046 touchscreen controller class. 

### XView
XView is a set of user interface classes that somewhat mimics the Mac OS's appearance.  XView and all of its subclasses are intended to be statically created.  Particular attention needs to be given to the XView hierarchy because the hierarchy determines both the draw order and how hit testing is performed.  By hiding/showing a view and its subviews, you should be able to emulate dynamically created view hierarchies such as the NSView classes on the Mac.

Commonly used XView subclasses are: XAlert, XCheckboxButton, XDateValueField, XDialogBox, XLabel, XMenuButton, XNumberValueField, XPopUpButton, XPushButton, XRadioButton, and XStepper.

XView subclasses use XFont and the drawing functions found in the DisplayController class.  DisplayController performs copy and fills, draws framed rectangles, rounded rectangles, antialiased lines and circles.  XFont uses the DisplayController to draw bitmapped fonts, both 1-bit and 8-bit antialiased.  In many DisplayController subclasses the fill functions are optimized for the target display.  Fill functions are the basis for all drawing.

To shorten development time, XView hierarchies can be viewed and debugged on a Mac by using the BitmapDisplayController subclass (not in this repository).  The same code that runs on the mcu also runs on the Mac.  This includes the XFont and DisplayController classes.  The ArduinoDisplayTester Xcode project is a modified version of the SubsetFontCreator project available in my SubsetFontCreator repository.  I don't plan on adding the ArduinoDisplayTester to a git repository.

### ST77XXToXPT2046Alignment
The XView subclass ST77XXToXPT2046Alignment is a utility used to align the display with the touch screen.  The result of the alignment session is saved as a preference that is loaded at startup.  Because prior to aligning the touchscreen, the touchscreen can't be used for precise input, the alignment session is initiated by pressing a button on the host board.  Currently the buttons on the host board serve no other purpose.

### SKeySpec and KeySpec
The SKeySpec struct encapsulates a single manufacture's keyway specification.  The KeySpec class reads a single SKeySpec from a text file.  The KeySpec class has been debugged but isn't used.  All SKeySpecs are currently hard coded.

### KeyMachineSTM32
KeyMachineSTM32 manages the UI, steppers, and motor control.  

### KMAction
Stepper movements and motor control are performed using subclasses of KMAction.  KMActions are added to the KMActionQueue and are executed in the order they were added.

**Actions:**
- CutKey calculates and executes the moves required to produce a key based on the specified SKeySpec, pin count, and cut depths.
- FastMoveTo moves a single stepper at high speed to a position.
- HomeEndstop homes a single endstop.
- CallbackAction calls a callback with optional wait periods before and after executing the callback.  This is currently used to stop and start the motor.  Without a callback this action can be used to insert a delay in the queue.

### STM32UnixRTC
STM32UnixRTC is a subclass of my UnixTime class.  STM32UnixRTC calls functions within rtc.c provided by STMicroelectronics.  I commented out the code within RTC\_init of rtc.c that truncates the 32-bit time storage within the mcu to only the number of seconds past midnight.  The STM32UnixRTC code needs the actual seconds, all 32 bits.  STM32UnixRTC does not use the STM time and date structs RTC\_TimeTypeDef, and RTC_DateTypeDef.  Instead STM32UnixRTC has its own version of ReadRTCCount and WriteRTCCount that accesses the time addresses directly.  This allows you to set and get the time as a 32-bit value and not have to pack and unpack the tedious DOS-like date and time structs.

See my 
[Key Code Cutter](https://www.instructables.com/KeyCodeCutter/) instructable for more information.
