# Arduino_Moonlite_Focuser
Arduino Focuser for a telescope, based on the Moonlite protocol, INDI compatible.

This software will control a motorfocuser that uses a dc geared motor, like SkyWatcher's DC focuser.

Hardware:
- SkyWatcher DC focuser or compatible, just the motor and the cable are needed. The handbox is replaced by the motor controller.
- Arduino UNO (will probably also work on other Arduino boards)
- (Velleman) motor driver shield, or any other Arduion compatible motor driver.
- Power source of your choice. The focuser can't run off the Arduino's power if it's powered from USB. You need a source that can power the DC motor. SkyWatcher's focuser runs at 9 V, but is supposedly rated at 12 V.

Software:
Install the GearedMotor files as a library in the Arduino IDE.
Upload the ino file to the Arduino UNO

The motor is connected to ports 9 (enable), 8 and 7 (direction) of the Arduino. The Velleman driver shield only uses ports 9 and 8.
Start INDIserver with the Moonlite focuser (indi_moonlite_focus)
The focuser has no speed control. Set the 'step' size to control the focusing action. The focuser can't tell at which position the software starts, so it supposes that position '0' is all the way in. Increase position by focusing outward. (Depends on how you wire the motor, of course.)
There's no temperature compensation.

The software was tested with INDI installed on a Raspberry Pi, focuser software on an Arduino UNO. Motor shield: Velleman motor shield for Arduino UNO. Motor: Skywatcher's DC focuser. Telescope: SkyWatcher's Explorer 150PDS. The autofocus routine of INDI was not tested yet (August 2017), due to lack of clear skies. On this setup, the maximum focus travel was at least 25000 steps. This should be tested and set in the appropriate INDI files.
