---
layout: page
title: Results
permalink: /results/
---
# Desired Specifications and Key Performance Aspects
- Successfully tracks user input from the Wii Nunchuk
   - MET! The MCU successfully loads in data bytes from the nunchuk relating to the sensors on the controller.
- Wrote a library for initializing and driving the LED matrix with the FPGA
  - MET! See code in the Documentation tab.
- Wrote a library for interfacing with the Wii Nunchuk adapter over I2C using the MCU
  - MET! See code in the Documentation tab.
- Display a maze on the LED matrix with wall collision physics
  - MET! The MCU handles the wall collision physics by checking intented player movements vs player position. During play, the player cannot phase through walls or outisde of the maze.
- Communicates user input from Wii Nunchuk to the LED matrix
  - MET! The MCU successfully decodes data from the nunchuk, updates and sends game/player data to the FPGA, and the FPGA sends the proper display data to the LED matrix.
- Winning/Losing animations play on LED matrix
  - MET! See animations section in the Design tab.
- Project is operational for a presentation to peers/professor
  - MET!
- Created a website clearly displays project details/results
  - MET!

# Results
With the completion of the specifications above, we were able to create a playable maze game inside its own game cabinet. The maze game has a timer that introduces a win/lose condition and allows the player to navigate the maze using the Wii Nunchuk controller. The LED matrix displays the maze and plays all our fun animations properly.

(bideo)
