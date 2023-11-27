// Martin Susanto
// msusanto@hmc.edu
// 12 November 2023
// DE-DP14211 16x32 LED Matrix Driver

#include "STM32L432KC.h"

void initDP14211(void);

void writeDP14211(uint16_t x, uint16_t y, uint16_t rg, uint16_t en);

void clearDP14211(void);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void checkDone(void);