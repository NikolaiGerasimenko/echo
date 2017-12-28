#include "stm32f30x.h"

#define UART_SPEED 9600
#define UART1_BAUD (SystemCoreClock/UART_SPEED)

volatile uint8_t flagRX = 0;

int main(void)
{
	volatile uint8_t dataBuf;
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	
	
	GPIOA->MODER |= GPIO_MODER_MODER9_1;		//1	
	GPIOA->MODER |= GPIO_MODER_MODER10_1;		//1

	GPIOA->AFR[1] |= (1<<4 | 1<<5 | 1<<6 | 1<<8 | 1<<9 | 1<<10);
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = UART1_BAUD;																			//Bodrate for 9600 on 72MHz	
	USART1->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;		//USART1 ON, TX ON, RX ON
	
	USART1->CR1 |= USART_CR1_RXNEIE;				// USART1 Receive Interrupt Enable.
	
	NVIC_EnableIRQ(USART1_IRQn);							// Enable interrupt from USART1(NVIC level) 
	NVIC_SetPriority(USART1_IRQn, 7);					// Set Interrupt Priority
	
	while(1)
	{
		if(flagRX == 1)
		{
			flagRX = 0;
			dataBuf = USART1->RDR;
			USART1->TDR = dataBuf;
		}
	}
}

void USART1_IRQHandler(void)
{		
	if(USART1->ISR & USART_ISR_RXNE)
		{
			USART1->ISR &= ~(USART_ISR_RXNE);
			flagRX = 1;
		}
}
