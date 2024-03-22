# Station

Designed for my personal weather station project, This is the software for the
station that directly manages the weather sensors and sends the data down to the
server. 

The project is designed for the [Teensy4.0], and itdepends on the [MS8607],
[SI1145], and several repurposed weather station sensors: a weather vane,
anemometer, and rain meter.

[Teensy4.0]: https://www.pjrc.com/store/teensy40.html
[MS8607]: https://www.adafruit.com/product/4716
[SI1145]: https://www.adafruit.com/product/1981

Communication with the base station is done through RS422 using a custom
protocol called [S-Code] which is inspired by [G-Code].

[S-Code]: https://github.com/ttocsneb/scode
[G-Code]: https://en.wikipedia.org/wiki/G-code

In this project there are several header files each with a specific purpose.

* [commands.hpp] - Handles scode commands
* [comms.hpp] - Operates the scode communications
* [date.hpp] - Implementation of datetime inspired by CANOpen's date object
* [debug.hpp] - Serial Debugging definitions
* [rain.hpp] - Rain Sensor
* [sensor.hpp] - MS8607/SI1145 Sensors
* [settings.hpp] - EEPROM settings
* [wind.hpp] - anemometer/vane sensor

[commands.hpp]: include/commands.hpp
[comms.hpp]: include/comms.hpp
[date.hpp]: include/date.hpp
[debug.hpp]: include/debug.hpp
[rain.hpp]: include/rain.hpp
[sensor.hpp]: include/sensor.hpp
[settings.hpp]: include/settings.hpp
[wind.hpp]: include/wind.hpp

There is a file called [scode.yml] which defines all of the scode commands which
are available to the communication interface.

[scode.yml]: scode.yml
