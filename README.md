# FreePulse Patient Monitor

This is the code base for [FreePulse][FreePulse], a low cost patient monitor
designed for developing world hospitals and resource-limited settings. Inspired
by my experiences working in rural hospitals in Rwanda and Nepal, FreePulse is
designed with the intention of providing hospital-grade diagnostic information
in a simple to use, durable package to doctors at an affordable price. A brief
overview of the project is available on the [project website][FreePulse], and
a more detailed post about the motivation and history of the project is on 
the [development blog][blog].

![alpha_prototype](resources/prototype.jpg)

## Current Goals and Notes

- Determine better SpO2 calibration timing (currently only calibrate after reset)

- Fix pulse oximeter readings (currently unstable). Develop a LUT or fitting
  strategy?

- Fix blood pressure inconsistencies. Look at patient data to determine where
  the failures lie.

- Add "recurring measurement" timer to NIBP module (i.e. "take a blood pressure
  measurement every five minutes" during surgery)

## Navigating the Repository

```
src
  |- Contains main.cpp
inc
  |- Interface classes 
  |- Signal-acquisition classes
libraries
  |- inc
	  |- Hardware-level wrapper classes
	  |- Peripheral library header files
  |- src
	  |- Peripheral library sources
linker
  |- Linker and library specifications
system
  |- System-level include configuration
tests
  |- Unit tests for all hardware-abstracted classes
```

[FreePulse]: http://www.freepulsemed.com
[blog]: http://reecestevens.me/blog
