# Automatic blinds
## Overview
Automatic blinds are a project based on The Atmel® AVR® ATmega32 low-power CMOS 8-bit microcontroller, which is based on the AVR enhanced RISC architecture. Its objective is to automate daily routines of opening and closing blinds depending on the specified time.

## Project specification
You can read this projects specification clicking the link below. 
```python
https://www.overleaf.com/read/ncnhqmkssbnk
```

## Pin and switch configurations
![Pinout configuration](https://github.com/dazeeeed/automatic-blinds/blob/main/images/pinout.png)

![Switch configuration](https://github.com/dazeeeed/automatic-blinds/blob/main/images/switches.png)

## Programming the controller
For use of this project the avc-gcc-10.1.0 compiler and zadig-2.5 USBAsp drivers were used. When everything is connected properly and the required software is installed program the controller using
```sh
make program
```
To clean the unused files run the command below.
```sh
make clean
```

## ATMega 32 manual
```python
https://ww1.microchip.com/downloads/en/DeviceDoc/doc2503.pdf
```

## License
[GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html)

