---
layout: page
title: Design
permalink: /design/
---

# MCU Design
  The Microcontroller talks to the Wii Nunchuck through I2C. Through two lines, Serial Clock and Serial Data, the microcontroller is able to recognize itself as the controller and the nunchuck as the receiver. The Microcontroller first sends out a start condition that flags the start of a communication request. Then, an address is sent out along with the type of request (read/write) and the MCU waits for an acknowledgement from the Nunchuk. After that, an initialization process needs to be completed by sending over 2 sets of 2 data bytes that allow the Nunchuck’s first and second registers to be used. Then, the sensor data on the Nunchuk can be read at any time by first sending the address and a handshake byte and then sending the address again along with the request and any relevant data bytes. For a read, the Nunchuck will send over its bytes of information to the Microcontroller. The data comes in the form of 6 bytes of data corresponding to the position of the joystick, the state of two buttons, the rotation of the controller, and the acceleration of the controller. A schematic of the whole system is shown below

<div style="text-align: left">
  <img src="../assets/schematics/E155 Labs - Project Schematic.jpeg" alt="logo1" width="900" />
</div>

Fig 1. System-level block diagram, including the Wii Nunchuk, MCU, FPGA, LED Matrix, and speaker.

When the write or read is complete, a stop condition is enforced, allowing the MCU to exit controller mode and wait for a communication to be requested again.
  Once the Nunchuck message is received, it is parsed to pull out the x and y positions of the joystick and the states of the two buttons on the controller. Once these inputs are decoded, the MCU translates them into player movements and/or menu interactions and updates the game state as well as the screen that will be displayed on the LED Matrix. The display data is sent to the FPGA over SPI according to the communication standard defined in the FGPA design section. The MCU then waits for the DONE signal from the FPGA, signaling that the FPGA is done shifting data into the display and that the MCU can send over the next message.

# FPGA Design


<div style="text-align: left">
  <img src="../assets/schematics/FPGA_controller_FSM.jpeg" alt="logo1" width="900" />
</div>

Fig 2. Finite state machine controller for the FPGA DE-DP14112 driver.



## FPGA Block Diagram
<div style="text-align: left">
  <img src="../assets/schematics/FPGA_block_diagram.jpeg" alt="logo1" width="900" />
</div>

Fig 3. Block diagram for the FPGA DE-DP14112 driver.

