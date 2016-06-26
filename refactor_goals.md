Refactor Goals
==============

1. Separate out display functionality, interface design, and related constants/methods from main

2. Abstract away GPIO, SPI, and other peripheral controls. Reference pins by their documentation name (i.e. PA1, PC5)

3. Unit test everything that can be unit tested.
