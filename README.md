# ClockSilencer
ClockSilencer is a simple ESP32 device. It plays recorded chime on certain hours.
## Why do I need that?
The problem is, I've got an antique pendulum clock from 1930s. When it chimes, it chimes all day long, day and night. I just wanted to silence it at night.
My first idea was to build a blocking mechanism inside the clock box, but it turns out to be very difficult and I had some doubts about the clock mechanism.
Much more elegant solution is having another electronic clock, that plays recorded chime on certain hours.

## Hardware:
- ESP32 WROOM
- DFPlayer mini
- micro SD card (a small one will be enough - 8gb or even less)

## Libraries:
- ESP32Time - Runs built-in RTC
- HardwareSerial - as DFPlayer is driven by Serial port, second serial must be set.
- DFRobotDFPlayerMini - driver for DFPlayer Mini

## Wiring:
[![Fritzing ESP32 schema](Untitled%20Sketch_bb.png)](Untitled%20Sketch_bb.png)

