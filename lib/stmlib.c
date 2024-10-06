/*
 * stmlib.c
 *
 *  Created on: Dec 25, 2023
 *      Author: Abhinav
 */

#include "stmlib.h"

void enable_hse(void) {
	RCC_t *RCC = (RCC_t*) RCC_BASE;
	FLASH_t *FLASH = (FLASH_t*) FLASH_BASE;
	FLASH->ACR |= 1 << 1;
	RCC->CR &= ~(1 << 18); // HSE not bypassed
	RCC->CR |= 1 << 16; // Enable HSE
	while (!(RCC->CR & 0x20000)){} // Wait till HSE gets ready
	RCC->CFGR &= ~(1 << 17); // HSE clock not div by 2
	RCC->CFGR |= 1 << 16; // HSE as PLL source
	RCC->CFGR |= 7 << 18; // PLL mul x9
	RCC->CR |= 1 << 24; // PLL ON
	while (!(RCC->CR & 0x2000000)){} // Wait till PLL gets ready
	RCC->CFGR |= 1 << 1; // PLL as system clock
	while (!(RCC->CFGR & 0x08)){} // PLL as system clock is ready
}

void systick_init(void)
{
	STK_t* STK = (STK_t*)SYSTEM_TIMER_BASE;
	STK->CTRL  = 0;
	STK->LOAD  = 0x00FFFFFF;
	STK->VAL   = 0;
	STK->CTRL |= 5;
}

void delaymillis(void)
{
	STK_t* STK = (STK_t*)SYSTEM_TIMER_BASE;
	STK->LOAD  = 0x11940;
	STK->VAL   = 0;
	while((STK->CTRL & 0x00010000) == 0);
}

void delayms(unsigned long t)
{
	for(;t>0;t--)
	{
		delaymillis();
	}
}

void hse_init(void)
{
	RCC_t* RCC = (RCC_t*)RCC_BASE;
	RCC->CFGR = 0;
	RCC->CR   = 0;
	RCC->CFGR |= 1; // HSE as system clock
	RCC->CR   |= (1 << 16); // HSE ON
	while ((RCC->CR & 0x20000) == 0){}
}

GPIO_t* get_gpio_base_addr(uint8_t port)
{
	GPIO_t *gpio;
	switch (port)
	{
	case GPIOA:
		gpio = (GPIO_t*) GPIOA_BASE;
		break;

	case GPIOB:
		gpio = (GPIO_t*) GPIOB_BASE;
		break;

	case GPIOC:
		gpio = (GPIO_t*) GPIOC_BASE;
		break;

	default:
		gpio = (GPIO_t*) NULL;
		break;
	}
	return gpio;
}

void stm_lib_init_gpio(uint8_t port)
{
	RCC_t  *RCC  = (RCC_t*) RCC_BASE;

	switch (port)
	{
	case GPIOA:
		RCC->APB2ENR |= (1 << 2);
		break;
	case GPIOB:
		RCC->APB2ENR |= (1 << 3);
		break;
	case GPIOC:
		RCC->APB2ENR |= (1 << 4);
		break;
	default:
		break;
	}
}

void stm_lib_config_gpio(uint8_t port, uint8_t pin, uint8_t mode)
{
	GPIO_t *gpio = get_gpio_base_addr(port);

	if ((pin >= 0) && (pin <= 7))
	{
		pin = pin * 4;
		gpio->CRL &= ~(1 << (pin));
		gpio->CRL &= ~(1 << (pin+1));
		gpio->CRL &= ~(1 << (pin+2));
		gpio->CRL &= ~(1 << (pin+3));
		gpio->CRL |= (mode << pin);
	}
	else if ((pin >= 8) && (pin <= 15))
	{
		pin = pin - 8;
		pin = pin * 4;
		gpio->CRH &= ~(1 << (pin));
		gpio->CRH &= ~(1 << (pin+1));
		gpio->CRH &= ~(1 << (pin+2));
		gpio->CRH &= ~(1 << (pin+3));
		gpio->CRH |= (mode << pin);
	}
}

void stm_lib_write_gpio(uint8_t port, uint8_t pin, uint8_t state)
{
	GPIO_t *GPIO = get_gpio_base_addr(port);

	state ? (GPIO->ODR |= (1 << pin)) : (GPIO->ODR &= ~(1 << pin));
}

uint8_t stm_lib_read_gpio(uint8_t port, uint8_t pin)
{
	GPIO_t* GPIO = get_gpio_base_addr(port);
	uint8_t value = 1;
	value <<= pin;
	value = (uint8_t)((GPIO->IDR) & value);
	value >>= pin;
	return value;
}

void stm_lib_toggle_gpio(uint8_t port, uint8_t pin)
{
	GPIO_t *GPIO = get_gpio_base_addr(port);
	uint16_t mask = 1;
	mask <<= pin;
	mask = (uint16_t)((GPIO->ODR) & mask);
	mask >>= pin;
	mask ? stm_lib_write_gpio(port, pin, 0) : stm_lib_write_gpio(port, pin, 1);
}

I2C_t* get_i2c_base_addr(uint8_t channel)
{
	if (channel == 1)
	{
		return (I2C_t*)I2C1_BASE;
	}

	return (I2C_t*)I2C2_BASE;
}


void stm_lib_init_I2C(uint8_t channel, uint16_t mode)
{
	RCC_t *RCC = (RCC_t*) RCC_BASE;
	I2C_t *I2C = get_i2c_base_addr(channel);
	RCC->APB2ENR = 1;

	switch (channel)
	{
	case 1:
		RCC->APB1ENR |= (1 << 21);
		stm_lib_init_gpio(GPIOB);
		stm_lib_config_gpio(GPIOB, 6, MODE_OUTPUT_50_AT_OD); // SCL
		stm_lib_config_gpio(GPIOB, 7, MODE_OUTPUT_50_AT_OD); // SDA
		break;

	case 2:
		RCC->APB1ENR |= (1 << 22);
		stm_lib_init_gpio(GPIOB);
		stm_lib_config_gpio(GPIOB, 10, MODE_OUTPUT_50_AT_OD); // SCL
		stm_lib_config_gpio(GPIOB, 11, MODE_OUTPUT_50_AT_OD); // SDA

	default:
		break;
	}

	I2C->CR1  |=  (1 << 15);
	I2C->CR1  &= ~(1 << 15);
	I2C->CR2   = 0x08;
	I2C->CCR   = mode;
	I2C->TRISE = 0x09;
	I2C->CR1  |= 1;
}

void stm_lib_i2c_start(uint8_t channel)
{
	I2C_t *I2C = get_i2c_base_addr(channel);
	I2C->CR1 |= 0x100;
	while (!(I2C->SR1 & 1)){}
}

void stm_lib_i2c_address(uint8_t channel, uint8_t address, uint8_t rw)
{
	volatile uint16_t temp;
	I2C_t *I2C = get_i2c_base_addr(channel);
	I2C->DR = (address | rw);
	while ((I2C->SR1 & 2) == 0){}
	while (I2C->SR1 & 2)
	{
		temp = I2C->SR1;
		temp = I2C->SR2;
		if ((I2C->SR1 & 2) == 0)
		{
			break;
		}
	}
	temp = temp;
}

void stm_lib_i2c_data(uint8_t channel, char data)
{
	I2C_t *I2C = get_i2c_base_addr(channel);
	while ((I2C->SR1 & 0x80) == 0){}
	I2C->DR = data;
	while ((I2C->SR1 & 0x80) == 0){}
}

void stm_lib_i2c_stop(uint8_t channel)
{
	volatile uint16_t temp;
	I2C_t *I2C = get_i2c_base_addr(channel);
	while ((I2C->SR1 & 0x04) == 0){}
	I2C->CR1 |= 0x200;
	while (I2C->SR1 & 0x04)
	{
		temp = I2C->SR1;
		I2C->DR = 0;
	}
	temp = temp;
}

void stm_lib_i2c_write(uint8_t channel, uint8_t address, char *data)
{
	uint16_t i = 0;
	stm_lib_i2c_start(channel);
	stm_lib_i2c_address(channel, address, 0);
	while (data[i] != 0)
	{
		stm_lib_i2c_data(channel, data[i]);
		i++;
	}
	stm_lib_i2c_stop(channel);
}

SPI_t* get_spi_base_addr(uint8_t channel)
{
	if (channel == 1)
	{
		return (SPI_t*)SPI1_BASE;
	}

	return (SPI_t*)SPI2_BASE;
}

void stm_lib_spi_init(uint8_t channel)
{
	RCC_t *RCC   = (RCC_t*) RCC_BASE;
	SPI_t *SPI   = get_spi_base_addr(channel);
	RCC->APB2ENR |= 1;
	switch (channel)
	{
	case 1:
		RCC->APB2ENR |= 0x1000;
		stm_lib_config_gpio(GPIOA, 5, MODE_OUTPUT_50_AT_PP); // SCK
		stm_lib_config_gpio(GPIOA, 6, MODE_INPUT_PULL);      // MISO
		stm_lib_config_gpio(GPIOA, 7, MODE_OUTPUT_50_AT_PP); // MOSI
		break;

	case 2:
		break;

	default:
		break;
	}
	

	SPI->CR1 |= 0x04; // Master mode
	//SPI->CR2 |= 1 << 1; // enable Tx dma
	SPI->CR2 |= 0x04; // SS output enable
	SPI->CR1 |= 0x40; // SPI enable	
}

void stm_lib_spi_tx(uint8_t channel, uint8_t data)
{
	SPI_t* SPI = get_spi_base_addr(channel);
	while (SPI->SR & 0x80){}
	while (SPI->SR & 0x02)
	{
		SPI->DR = data;
	}
}

void stm_lib_spi_tx_dma(uint8_t channel, uint8_t data, uint8_t *mem)
{
	DMA_t *DMA = (DMA_t*) DMA1_BASE;
	SPI_t *SPI = get_spi_base_addr(channel);
	DMA->IFCR |= (1 << 9); // clear DMA tx flag
	DMA->CPAR3 = SPI1_BASE + 0x0C; // set SPI DR
	*mem = data;
	DMA->CMAR3 = mem; // set mem location
	DMA->CNDTR3 = 1; // set total num of data
	DMA->CCR3 |= 0x01; // activate the channel
	while (!(DMA->ISR & 0x200)){} // wait till DMA tx completes
	DMA->IFCR |= (1 << 9); // clear DMA tx flag
	DMA->CCR3 &= ~(1 << 0); // deactivate the channel
}

uint8_t stm_lib_spi_rx(uint8_t channel, uint8_t value)
{
	uint8_t data;
	SPI_t* SPI = get_spi_base_addr(channel);
	SPI->DR = value;
	while (SPI->SR & 0x80) {}
	while(SPI->SR & 0x01)
	{
		data = SPI->DR;
	}
	return data;
}

unsigned long uart_baud(unsigned short usart, unsigned long BR)
{
	unsigned long div = 8000000UL;
	unsigned long dec;
	unsigned long final;
	double frac = 8000000.00;
	double frac2 = 1.00;
	
	if(usart == 1)
	{
	div = 72000000UL;
	frac = 72000000.00;
	}
	div = div / (BR*16);
	frac = 16*((frac / (BR*16))-div);
	dec = frac;
	frac2 = 100*(frac-dec);
	if(frac2>50)
	{
		dec ++;
		if(dec == 16)
		{
			dec = 0;
			div ++;
		}
	}
	
	final = (div<<4);
	final += dec;
	
	return final;
	
}

void uart_init(uint8_t channel, unsigned long baud)
{
	unsigned long b = uart_baud(2, baud);
	UART_t *UART = (UART_t*) USART1_BASE;
	RCC_t *RCC = (RCC_t*) RCC_BASE;
	RCC->APB2ENR |= 0x01;
	RCC->APB2ENR |= 0x4000;
	stm_lib_config_gpio(GPIOA, 9, MODE_OUTPUT_50_AT_PP);
	stm_lib_config_gpio(GPIOA, 10, MODE_INPUT_PULL);
	UART->BRR = b;
	UART->CR1 |= 8;
	UART->CR1 |= 4;
	UART->CR1 |= 0x2000;
}

void uart_putc(char c)
{
	UART_t *UART = (UART_t*) USART1_BASE;
	while(!(UART->SR & 0x80)){};
	UART->DR = c;
}

char uart_getc(uint8_t channel)
{
	char c;
	UART_t *UART = (UART_t*) USART1_BASE;
	while(!(UART->SR & 0x20)){};
	c = UART->DR;
	return c;
}

void uart_puts(char *str)
{
	while (*str != 0)
	{
		uart_putc(*str++);
	}
}

void dma_init(uint8_t channel, uint8_t *mem)
{
	DMA_t *DMA   = (DMA_t*) DMA1_BASE;
	RCC_t *RCC   = (RCC_t*) RCC_BASE;
	RCC->AHBENR |= 0x01;     // enable DMA1 clock
	DMA->CCR3   |= (1 << 4); // read from memory
	DMA->CCR3   |= (1 << 7); // mem inc mode
	DMA->CCR3   |= (1 << 1); // enable TCIe
}

uint8_t stm_lib_read_btn(btn_t* btn) {
	return stm_lib_read_gpio(btn->port, btn->pin);
}



