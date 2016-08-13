#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_usart.h"

USART_HandleTypeDef ghUsart2;

void HAL_USART_MspInit(USART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_USART2_CLK_ENABLE();
	/* configure PD5/6 for UART2 */
	GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

	HAL_NVIC_SetPriority(USART2_IRQn, 1, 3);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

}

/*
 * GPIO: GPIO_D_5/6
 * Intr: USART2_IRQn
 *
 */
void MyUart_Configure(void)
{
	ghUsart2.Init.BaudRate = 115200;
	ghUsart2.Init.WordLength = UART_WORDLENGTH_8B;
	ghUsart2.Init.Parity = UART_PARITY_NONE;
	ghUsart2.Init.StopBits = UART_STOPBITS_1;
	//ghUsart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	ghUsart2.Init.Mode = UART_MODE_TX_RX;
	ghUsart2.Instance = USART2;
	HAL_USART_Init(&ghUsart2);
	//__HAL_UART_ENABLE_IT( &ghUsart2, UART_IT_TC);
	//__HAL_UART_ENABLE(&ghUsart2);
}


static int total_tx_cnt = 0;
void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart)
{
    total_tx_cnt++;
}
int MyUsart_SendDataSync(uint8_t *data, int siz)
{
	HAL_USART_Transmit(&ghUsart2, data, siz, 10);
    //HAL_USART_Transmit_IT(&ghUsart2, data, siz);
	return 0;
}
