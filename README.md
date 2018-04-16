# PhoneBoothRinger

This project uses a commodity STM32F103C8 microcontroller board to detect motion
and activate the ringer in a payphone, stopping when the phone is replaced on
the hook

It was built using [platformio](https://platformio.org), and all configuration required for successful
building and flashing are included. You will need an ST-Link programmer to
upload the program to the microcontroller.

The ringer output is wired to a NPN BJT connected on one of the secondaries of a
12:120V Transformer, the other secondary is connected to a 9V rail via a 70ohm
resistor. You will want to play with your resistor values based on the impedance
of the transformer you're using.
