/*
File: STM32L432KC_I2C.c
Authors: Lawrence Nelson and Cecilia Li
Email: llnelson@g.hmc.edu, celi@g.hmc.edu

This code defines the functions to initialize I2C communication
with the Wii Nunchuk and read its data
*/

#include "STM32L432KC.h"

void initI2C(void) {
  
  // Configure and enable I2C Peripheral Clock in RCC
  RCC->CFGR |= _VAL2FLD(RCC_CFGR_PPRE1,0b101); // 8MHz
  RCC->CFGR |= _VAL2FLD(RCC_CFGR_HPRE,0b0000);
  RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
  RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_I2C1SEL,0b00);

  // Disable I2C for configuration
  I2C1->CR1 &= ~I2C_CR1_PE;
  
  // Set pins to standard Speed
  SYSCFG->CFGR1 &= ~SYSCFG_CFGR1_I2C1_FMP;
  SYSCFG->CFGR1 &= ~SYSCFG_CFGR1_I2C_PB6_FMP;

  // Turn Analog Filter Off
  I2C1->CR1 |= I2C_CR1_ANFOFF;

  // Turn Digital Filter Off
  I2C1->CR1 &= ~(I2C_CR1_DNF);
  
  // Configure Timing for 100kHz communication in standard mode
  I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_PRESC,0xB);
  I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SCLDEL,0x4);
  I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SDADEL,0x2);
  I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SCLH,0xF);
  I2C1->TIMINGR |= _VAL2FLD(I2C_TIMINGR_SCLL,0x13);

  // Enable interrupt flags
  I2C1->CR1 |= I2C_CR1_RXIE;
  I2C1->CR1 |= I2C_CR1_TXIE;
  I2C1->CR1 |= I2C_CR1_NACKIE;
  I2C1->CR1 |= I2C_CR1_STOPIE;

  // Turn off receiver clock stretching
  I2C1->CR1 &= ~(I2C_CR1_NOSTRETCH);

  // Set end commands; hardware controlled
  I2C1->CR2 |= I2C_CR2_AUTOEND;
  I2C1->CR2 &= ~I2C_CR2_RELOAD;

  // Enable I2C
  I2C1->CR1 |= I2C_CR1_PE;

  // Send clock to GPIOA/B
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);

  // Initially assigning I2C pins
  pinMode(I2C_SCL, GPIO_ALT);     // I2C1_SCL
  pinMode(I2C_SDA, GPIO_ALT);     // I2C1_SDA

  // Set to AF04 for I2C alternate functions
  GPIOA->AFR[1] |= _VAL2FLD(GPIO_AFRH_AFSEL9, 4);
  GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL7, 4);

  // Set output type to open-drain
   GPIOA->OTYPER |= GPIO_OTYPER_OT9;
   GPIOB->OTYPER |= GPIO_OTYPER_OT7;
}

void initNunchukFirst(void){
  // Send initialization bytes to nunchuk to set up data registers
  
  while(I2C1->CR2 & I2C_CR2_START_Msk); // delay for nbytes setting
  
  // Set number of bytes for transfer
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES,0x02); // sending 2 initialization bytes

  // Outline from p 1152
  char data[2] = {0xF0, 0x55};
  int i = 0;
  char addr = 0xA4;

  // Set address mode, 7-BIT MODE 
  I2C1->CR2 &= ~I2C_CR2_ADD10;

   // clear nack flag
  I2C1->ICR |= I2C_ICR_NACKCF;

  // Set address  0x52 -> 0b0101_0010 -> 0b010_1001
  I2C1->CR2 &= ~I2C_CR2_SADD;
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD,(addr)); // I2C adress of the Wii Nunchuk x52

  // Set transfer direction for a write
  I2C1->CR2 &= ~I2C_CR2_RD_WRN;

  // wait for tx buffer to clear
  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

  // Set START bit
  I2C1->CR2 |= I2C_CR2_START;

  // put data in tx buffer
  *(volatile char *) (&I2C1->TXDR) = data[0];

  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

  // put data in tx buffer
  *(volatile char *) (&I2C1->TXDR) = data[1];

  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));  
}

void initNunchukSecond(void){
  // Send more initialization bytes to nunchuk to put data in registers for reading
  while(I2C1->CR2 & I2C_CR2_START_Msk); // delay for nbytes setting
  
  // Set number of bytes for transfer
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES,0x02); // sending 2 initialization bytes

  // Outline from p 1152
  char data[2] = {0xFB,0x00};
  int i = 0;
  char addr = 0xA4;

  // Set address mode, 7-BIT MODE 
  I2C1->CR2 &= ~I2C_CR2_ADD10;

   // clear nackf
  I2C1->ICR |= I2C_ICR_NACKCF;

  // Set address  0x52 -> 0b0101_0010 -> 0b010_1001
  I2C1->CR2 &= ~I2C_CR2_SADD;
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD,(addr)); // I2C adress of the Wii Nunchuk x52

  // Set transfer direction for a write
  I2C1->CR2 &= ~I2C_CR2_RD_WRN;

  // wait for tx buffer to clear
  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

  // Set START bit
  I2C1->CR2 |= I2C_CR2_START;

  // put data in tx buffer
  *(volatile char *) (&I2C1->TXDR) = data[0];

  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

   // put data in tx buffer
  *(volatile char *) (&I2C1->TXDR) = data[1];

  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));
}

void initNunchukThird(void){
  while(I2C1->CR2 & I2C_CR2_START_Msk); // delay for nbytes setting
  
  // Set number of bytes for transfer
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES,0x01); // sending 2 initialization bytes

  // Outline from p 1152
  char data[1] = {0xFA};
  int i = 0;
  char addr = 0xA4;

  // Set address mode, 7-BIT MODE 
  I2C1->CR2 &= ~I2C_CR2_ADD10;

   // clear nackf
  I2C1->ICR |= I2C_ICR_NACKCF;

  // Set address  0x52 -> 0b0101_0010 -> 0b010_1001
  I2C1->CR2 &= ~I2C_CR2_SADD;
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD,(addr)); // I2C adress of the Wii Nunchuk x52

  // Set transfer direction for a write
  I2C1->CR2 &= ~I2C_CR2_RD_WRN;

  // wait for tx buffer to clear
  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

  // Set START bit
  I2C1->CR2 |= I2C_CR2_START;

  // put data in tx buffer
  *(volatile char *) (&I2C1->TXDR) = data[0];

  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));
}

void initNunchukPrim(void){
  // Tells nunchuk to decode data and place into address register
  while(I2C1->CR2 & I2C_CR2_START_Msk); // delay for nbytes setting
  
  // Set number of bytes for transfer
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES,0x01); // sending 2 initialization bytes

  // Outline from p 1152
  char data[1] = {0x00};
  int i = 0;
  char addr = 0xA4;

  // Set address mode, 7-BIT MODE 
  I2C1->CR2 &= ~I2C_CR2_ADD10;

   // clear nackf
  I2C1->ICR |= I2C_ICR_NACKCF;

  // Set address  0x52 -> 0b0101_0010 -> 0b010_1001
  I2C1->CR2 &= ~I2C_CR2_SADD;
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD,(addr)); // I2C adress of the Wii Nunchuk x52

  // Set transfer direction for a write
  I2C1->CR2 &= ~I2C_CR2_RD_WRN;

  // wait for tx buffer to clear
  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

  // Set START bit
  I2C1->CR2 |= I2C_CR2_START;

  // put data in tx buffer
  *(volatile char *) (&I2C1->TXDR) = data[0];

  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));
}


char * readData(void){
  // Access data register in nunchuk and read bytes
  while(I2C1->CR2 & I2C_CR2_START_Msk); // delay for nbytes setting

  // Outline from p 1156
  int i = 0;
  static char data[6];  // wii nunchuk sends 6 data bytes
  char addr = 0xA4;

  // Set address mode, 7-BIT MODE 
  I2C1->CR2 &= ~I2C_CR2_ADD10;

  // clear nackf and stop flag
  I2C1->ICR |= I2C_ICR_NACKCF;
  I2C1->ICR |= I2C_ICR_STOPCF;

  // Set address  0x52 -> 0b0101_0010 -> 0b010_1001
  I2C1->CR2 &= ~I2C_CR2_SADD;
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD,(addr)); // I2C adress of t

  // Set transfer direction for a read
  I2C1->CR2 |= I2C_CR2_RD_WRN;

  // wait for the read bit to be written
  while(!(I2C_CR2_RD_WRN));

  // wait for the buffer
  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

  // Set number of bytes for transfer
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES,0x06); // wii nunchuk sends 6 data bytes

  // Set START bit
  I2C1->CR2 |= I2C_CR2_START;

  // waiting for something to fill into the RXNE block
  while(!(I2C1->ISR & I2C_ISR_RXNE));

  data[0] = (volatile char) I2C1->RXDR;
  while(!(I2C1->ISR & I2C_ISR_RXNE));


  while(!(I2C1->ISR & I2C_ISR_RXNE));
  data[1] = (volatile char) I2C1->RXDR;


  while(!(I2C1->ISR & I2C_ISR_RXNE));
  data[2] = (volatile char) I2C1->RXDR;


  while(!(I2C1->ISR & I2C_ISR_RXNE));
  data[3] = (volatile char) I2C1->RXDR;

  while(!(I2C1->ISR & I2C_ISR_RXNE));

  data[4] = (volatile char) I2C1->RXDR;

  while(!(I2C1->ISR & I2C_ISR_RXNE));

  data[5] = (volatile char) I2C1->RXDR;

  I2C1->ICR |= I2C_ICR_STOPCF;
  
  return data;
}
