---
layout: page
title: Documentation
permalink: /doc/
---

# Schematics
<!-- Include images of the schematics for your system. They should follow best practices for schematic drawings with all parts and pins clearly labeled. You may draw your schematics either with a software tool or neatly by hand. -->
The following schematic shows how all five components of our system interact with each other. The Wii Nunchuk connects to a breakout board to allow for easy access to the different I2C signals on a breadboard or protoboard. The breakout board is then connected to two pins on the STM32 Nucleo-32 MCU that are programmable for I2C communication. All controller data between the MCU and Nunchuk is sent through these two lines. The MCU drives the DE-DP14112 LED matrix by sending data to the FPGA over SPI. The DE-DP14112 uses a non-standard serial interface, so the FPGA acts as a "translator" between SPI messages from the MCU and messages to be sent to the display. Displayable game data is sent to the matrix at a speed of 1 MHz.

<div style="text-align: left">
  <img src="../assets/schematics/E155 Labs - Project System Block Diagram.jpeg" alt="logo" width="900" />
</div>

## New Hardware

### Wii Nunchuk
We purchased a knock off Wii Nunchuk and a breakoutboard from Adafruit. The board takes the special STEMMA QT connection on the controller and breaks out the necessary pins for power and I2C communication. The I2C communication is done using our MCU's I2C peripheral which was not covered in class. The controller senses joystick position, controller rotation, and the state of two buttons

### 32x16 LED Dot Matrix 
This LED matrix was found in the digital lab. It comes with microcontrollers on board that run on their own version of SPI. It is made up of eight segmented LED panels that lend themselves nicely to mulitplexing. In order to run the display properly, we had to apply SPI and multiplexing together in a creative way to satisfy its communication protocol. 

# Source Code Overview
<!-- This section should include information to describe the organization of the code base and highlight how the code connects. -->
The source code for the project is located in the Github repository [here](https://github.com/Martin5009/amazing_game).

The FPGA folder includes a Lattice Radiant Project that contains HDL for an LED matrix driver. The MCU folder includes the library files and source files for a SEGGER ARM project that programs the MCU to run the game, take in and decode inputs from the Wii Nunchuk, and commands the FPGA to update the matrix.

# Bill of Materials
<!-- The bill of materials should include all the parts used in your project along with the prices and links.  -->

| Item | Part Number | Quantity | Unit Price | Link |
| ---- | ----------- | ----- | ---- | ---- |
| Adafruit Wii Nunchuck Breakout Adapter - Qwiic / STEMMA QT |  4836 | 1 | $2.95 |  [link](https://www.adafruit.com/product/4836) |
| Adafruit Wii Nunchuk Controller |  342 | 1 | $12.50 |  [link](https://www.adafruit.com/product/342) |
| Adafruit 32x16 Red Green Dual Color LED Dot Matrix - 7.62mm Pitch - DE-DP14211 | 3054 | 1 | N/A | [link](https://www.adafruit.com/product/3054) |
| STM32L432KC MCU | N/A | 1 | N/A | N/A |
| UPDUINO v3.0 FPGA | N/A | 1 | N/A | N/A |
| GF0668B Speaker | N/A | 1 | N/A | N/A |


**Total cost: $15.45**
