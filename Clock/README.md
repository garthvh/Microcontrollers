# Arduino Clock

![Clock Body](https://garthvh.com/assets/img/arduino_clock/clock_body.jpg)

I don't really have a great plan for this project, I already had a 4x20 LCD and 1307 breakout, I bought a $3 prototype [laser cut case](http://www.retrobuiltgames.com/the-build-page/arduino-clock-case/) on a whim and ordered some buttons to fit from china.

I recently ordered a bunch of $5 SMD Uno clones with a micro USB connection that come without headers soldered on. I am going to use one of these to liberate my Adafruit metro from my Ciclop 3D Scanner and I soldered some male headers onto one for use in the clock. This has allowed me to connect everything together using jumper wires.

Currently the clock and sketch supports the following hardware

* [$5 SMD Uno Clone](http://www.dxsoul.com/product/improved-version-uno-r3-atmega328p-development-board-parts-compatible-with-arduino-yellow-901426437)
* [Retro Built Games Laser Cut Case](http://www.retrobuiltgames.com/the-build-page/arduino-clock-case/)
* [3 Seeedstudio 1MM Domed Push Buttons](http://www.dx.com/p/seeedstudio-12mm-domed-push-buttons-set-black-white-6-pcs-399276)
* [DS1307 Module](http://www.dx.com/p/i2c-rtc-ds1307-24c32-real-time-clock-module-for-arduino-blue-149493)

The case has a cutout for a rocker switch, and a three way switch is pictured in the Retro Built Games photos.  I currently have a two way rocker switch installed in the case but don't have it actually connected and have not used it in any code yet.

![Clock Body](https://garthvh.com/assets/img/arduino_clock/clock_top.jpg)

To Start with I added a an enum to store the "modes" for the clock.  Pushing button 1 will switch to the next mode.  There are currently 4 different modes configured in the sketch.

* SHOW_SPLASH - Displays the Splash Screen for 5 seconds and then switches to SHOW_TIME
* SHOW_TIME - Displays the time using a large 3 row font below the date in the first row
* SHOW_CUSTOM_TEXT - Displays the time and custom text loaded from the serial port
* SHOW_SYSTEM_INFO - Displays System Information
 
![Clock Screen Time](https://garthvh.com/assets/img/arduino_clock/clock_screen_time.jpg)

SHOW_TIME Display

![Clock Screen Custom Prompt](https://garthvh.com/assets/img/arduino_clock/clock_screen_custom_1.jpg)

SHOW_CUSTOM_TEXT Prompt Screen

![Clock Screen Custom Text](https://garthvh.com/assets/img/arduino_clock/clock_screen_custom_2.jpg)

SHOW_CUSTOM_TEXT Display Screen

![Clock Screen System Information](https://garthvh.com/assets/img/arduino_clock/clock_screen_info.jpg)

SHOW_SYSTEM_INFO Display

The SHOW_SPLASH and SHOW_SYSTEM_INFO modes are essentially the same at this point, but the SHOW_SPLASH mode can be set as the default and then will load SHOW_TIME after the splash text has been up for 5 seconds. 
