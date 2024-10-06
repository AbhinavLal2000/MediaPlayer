/*
 * stmlib.h
 *
 *  Created on: Dec 25, 2023
 *      Author: Abhinav
 */

#ifndef STMLIB_H_
#define STMLIB_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SPI1 1
#define SPI2 2

#define SYSTEM_TIMER_BASE (0xE000E010UL)
#define GPIOA_BASE (0x40010800UL)
#define GPIOB_BASE (0x40010C00UL)
#define GPIOC_BASE (0x40011000UL)
#define RCC_BASE (0x40021000UL)
#define SPI1_BASE (0x40013000UL)
#define SPI2_BASE (0x00000000UL)
#define I2C1_BASE (0x40005400UL)
#define I2C2_BASE (0x40005800UL)
#define USART1_BASE (0x40013800UL)
#define DMA1_BASE (0x40020000UL)
#define DMA2_BASE (0x40020400UL)
#define FLASH_BASE (0x40022000UL)

#define GPIOA 0
#define GPIOB 1
#define GPIOC 2

#define MODE_INPUT_ANALOG 0
#define MODE_INPUT_FLOAT 4
#define MODE_INPUT_PULL 8

#define MODE_OUTPUT_10_GP_PP 1
#define MODE_OUTPUT_10_GP_OD 5
#define MODE_OUTPUT_10_AT_PP 9
#define MODE_OUTPUT_10_AT_OD 13

#define MODE_OUTPUT_2_GP_PP 2
#define MODE_OUTPUT_2_GP_OD 6
#define MODE_OUTPUT_2_AT_PP 10
#define MODE_OUTPUT_2_AT_OD 14

#define MODE_OUTPUT_50_GP_PP 3
#define MODE_OUTPUT_50_GP_OD 7
#define MODE_OUTPUT_50_AT_PP 11
#define MODE_OUTPUT_50_AT_OD 15

#define I2C_FM 0x2D
#define I2C_SM 0xB4

typedef struct
{
	uint32_t ACR;
	uint32_t KEYKR;
	uint32_t OPTKEYR;
	uint32_t SR;
	uint32_t CR;
	uint32_t AR;
	uint32_t reserved;
	uint32_t OBR;
	uint32_t WRPR;

} __attribute__((packed)) FLASH_t;

typedef struct
{
	uint32_t CRL;
	uint32_t CRH;
	uint32_t IDR;
	uint32_t ODR;

} __attribute__((packed)) GPIO_t;

typedef struct
{
	uint32_t CR;
	uint32_t CFGR;
	uint32_t CIR;
	uint32_t APB2RSTR;
	uint32_t APB1RSTR;
	uint32_t AHBENR;
	uint32_t APB2ENR;
	uint32_t APB1ENR;
	uint32_t BDCR;
	uint32_t CSR;

} __attribute__((packed)) RCC_t;

typedef struct
{
	uint32_t CR1;
	uint32_t CR2;
	uint32_t SR;
	uint32_t DR;
	uint32_t CRCPR;
	uint32_t RXCRCR;
	uint32_t TXCRCR;
	uint32_t I2SCFGR;
	uint32_t I2SPR;

} __attribute__((packed)) SPI_t;

typedef struct
{
	uint32_t CR1;
	uint32_t CR2;
	uint32_t OAR1;
	uint32_t OAR2;
	uint32_t DR;
	uint32_t SR1;
	uint32_t SR2;
	uint32_t CCR;
	uint32_t TRISE;

} __attribute__((packed)) I2C_t;

typedef struct
{
	uint32_t CTRL;
	uint32_t LOAD;
	uint32_t VAL;
	uint32_t CALIB;

} __attribute__((packed)) STK_t;

typedef struct
{
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;

} __attribute__((packed)) UART_t;

typedef struct 
{
	volatile uint32_t ISR;
	volatile uint32_t IFCR;
	volatile uint32_t CCR1;
	volatile uint32_t CNDTR1;
	volatile uint32_t CPAR1;
	volatile uint32_t CMAR1;
	volatile uint32_t RESERVED0;
	volatile uint32_t CCR2;
	volatile uint32_t CNDTR2;
	volatile uint32_t CPAR2;
	volatile uint32_t CMAR2;
	volatile uint32_t RESERVED1;
	volatile uint32_t CCR3;
	volatile uint32_t CNDTR3;
	volatile uint32_t CPAR3;
	volatile uint32_t CMAR3;
	volatile uint32_t RESERVED2;
	volatile uint32_t CCR4;
	volatile uint32_t CNDTR4;
	volatile uint32_t CPAR4;
	volatile uint32_t CMAR4;
	volatile uint32_t RESERVED3;
	volatile uint32_t CCR5;
	volatile uint32_t CNDTR5;
	volatile uint32_t CPAR5;
	volatile uint32_t CMAR5;
	volatile uint32_t RESERVED4;
	volatile uint32_t CCR6;
	volatile uint32_t CNDTR6;
	volatile uint32_t CPAR6;
	volatile uint32_t CMAR6;
	volatile uint32_t RESERVED5;
	volatile uint32_t CCR7;
	volatile uint32_t CNDTR7;
	volatile uint32_t CPAR7;
	volatile uint32_t CMAR7;
	volatile uint32_t RESERVED6;

} __attribute__((packed)) DMA_t;

typedef struct
{
	uint8_t port;
	uint8_t pin;

} btn_t;

void hse_init(void);
void enable_hse(void);
void systick_init(void);
void delaymillis(void);
void delayms(unsigned long);
GPIO_t *get_gpio_base_addr(uint8_t);
void stm_lib_init_gpio(uint8_t);
void stm_lib_config_gpio(uint8_t, uint8_t, uint8_t);
void stm_lib_write_gpio(uint8_t, uint8_t, uint8_t);
uint8_t stm_lib_read_gpio(uint8_t, uint8_t);
void stm_lib_toggle_gpio(uint8_t port, uint8_t pin);
I2C_t *get_i2c_base_addr(uint8_t);
void stm_lib_init_i2c(uint8_t, uint16_t);
void stm_lib_i2c_start(uint8_t);
void stm_lib_i2c_address(uint8_t, uint8_t, uint8_t);
void stm_lib_i2c_data(uint8_t, char);
void stm_lib_i2c_stop(uint8_t);
void stm_lib_i2c_write(uint8_t, uint8_t, char *);
SPI_t *get_spi_base_addr(uint8_t);
void stm_lib_spi_init(uint8_t);
void stm_lib_spi_tx(uint8_t, uint8_t);
void stm_lib_spi_tx_dma(uint8_t channel, uint8_t data, uint8_t *mem);
uint8_t stm_lib_spi_rx(uint8_t, uint8_t);
unsigned long uart_baud(unsigned short usart, unsigned long BR);
void uart_init(uint8_t channel, unsigned long baud);
void uart_putc(char c);
void uart_puts(char *str);
void dma_init(uint8_t channel, uint8_t *mem);
uint8_t stm_lib_read_btn(btn_t*);

#endif /* STMLIB_H_ */
