CableRelease
============

This contains all the files used in my Cable Release project. 
It is dependent on /Library which contains a library of peripheral code for use on a tiny 4313. 
And then /XanderCameraDevice contains project specific code and Makefile.

If you do decide to use my 4313 code for yourself, the LCD code in Library isn't completely portable. There are pins
used as GPIOs that are hardware-specific to this particular project. You have to go into /Library/LCD.? and change
them for yourself.
