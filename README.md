# amazing_game
Source code for a **timed maze runner game** implemented on a STM32432KC Microcontroller, using an iCE40 UP5K FPGA to drive a DE-DP14112 LED Matrix. User inputs are handled by a 3rd-party Wii Nunchuk, which communicates with the MCU over I2C.

## Structure
* [FPGA](https://github.com/Martin5009/amazing_game/tree/master/FPGA) contains SystemVerilog source code for the LED matrix driver, and a ModelSim project file for testing the design. The FPGA acts as a "translator" between the MCU and the LED matrix. The MCU sends a command to light up the matrix to the FPGA over SPI, then the FPGA sends that command to the matrix using a custom SPI-like interface unique to the DE-DP14112.
* [MCU](https://github.com/Martin5009/amazing_game/tree/master/MCU) contains C source code for the game logic, Wii Nunchuk driver, and matrix driver. The MCU receives user input from the Nunchuk over I2C and sends commands to light up the matrix over SPI to the FPGA.
* [sketchmatrix](https://github.com/Martin5009/amazing_game/tree/master/sketchmatrix) contains a Python script to generate bitmaps for arbitrary images to be drawn on the matrix. 

