---
layout: page
title: Documentation
permalink: /doc/
---

# Schematics
<!-- Include images of the schematics for your system. They should follow best practices for schematic drawings with all parts and pins clearly labeled. You may draw your schematics either with a software tool or neatly by hand. -->
The following schematic shows how all five components of our system interact with each other. The purchased Wii Nunchuk connects to a breakout board that allows for the I2C communication pins in the nunchuk to be landed on a breadboard or protoboard. The breakout board is then connected to two pins on our STM32 Nucleo-32 that are programmable for I2C communication. All controller data between the MCU and Nunchuk is sent through these two lines. From the MCU, four pins are connected to our iCE40 UP5K in order to establish SPI communication as well as a done signal. All game data from the MCU is sent through this connection to the FPGA. Finally, four pins on the FPGA are sent to our DE-DP14113 LED matrix to establish a psuedo-SPI protocol that is specific to our LED matrix. Displayable game data is sent to the matrix at a speed of 1 MHz.
<div style="text-align: left">
  <img src="./assets/schematics/E155 Labs - Project Schematic.jpeg" alt="logo" />
</div>

# Source Code Overview
<!-- This section should include information to describe the organization of the code base and highlight how the code connects. -->
The source code for the project is located in the Github repository [here](https://github.com/Martin5009/amazing_game).

The code is separated between the FPGA and the MCU. The FPGA folder includes a Radiant Project folder that contains all the code used to create a generic driver for our LED matrix. The MCU folder includes the library files and source files for a SEGGER ARM project that programs the MCU to run the game, take in and decode inputs from the Wii Nunchuk, and send game information to the FPGA.

# Bill of Materials
<!-- The bill of materials should include all the parts used in your project along with the prices and links.  -->

| Item | Part Number | Quantity | Unit Price | Link |
| ---- | ----------- | ----- | ---- | ---- |
| Adafruit Wii Nunchuck Breakout Adapter - Qwiic / STEMMA QT |  4836 | 1 | $2.95 |  [link](https://www.adafruit.com/product/4836) |
| Adafruit Wii Nunchuk Controller |  342 | 1 | $12.50 |  [link](https://www.adafruit.com/product/342) |
| Adafruit 32x16 Red Green Dual Color LED Dot Matrix - 7.62mm Pitch - DE-DP14211 | 3054 | 1 | $39.95 | [link](https://www.adafruit.com/product/3054) |
| STM32L432KC MCU | N/A | 1 | N/A | N/A |
| UPDUINO v3.0 FPGA | N/A | 1 | N/A | N/A |


**Total cost: $55.40**
