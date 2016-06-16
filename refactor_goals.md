Refactor Goals
==============

1. Move Arduino compatibility functions (i.e. digitalWrite, analogRead, etc) to a separate class
	- Leave the original adafruit libraries as intact as possible

2. Revisit the screen element / interface / ecg_revised division. Restructure and give better function, variable names.
	- "Functions should only do one thing, and do it well"

3. Unit testing via the Google Testing Framework
