#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "m.h"
//#include "./led/bsp_led.h"

#define DELAY_TIME_MIN  0x080000
#define DELAY_TIME      0x200000	// about 150ms
#define DELAY_TIME_MAX  0x800000
#define DELAY_TIME_STEP 0x080000

static int32_t delay_time = DELAY_TIME;
static UART_HandleTypeDef ghuart;
static int usart_tx_flag = 0; // 0: tx completed, 1: has tx (not completed)

/* for nCount = 0x20_0000, delay time is around 150ms */
inline void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

#if 0
static void MyCommonGPIO_Config(void)
{

    // Key1: GPIOA Pin0
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_3;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Alternate = 0;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /*
     * GPIOA: Pin0 (input)  -> Key1
     *        Pin3 (output) -> to control peripheral
     */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}

static void PowerOn(void)
{

}
#endif

/*
 * Set NVIC for Key1, Key2 and TIM6
 */
void MyNVICConfigure(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_1);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	

	HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

//static uint32_t freq = 0;

int main(void)
{
    int i;
    uint32_t data[10];
	//char data[] = "hello world!";
    MyHSE_SetSysClock();

    //freq = HAL_RCC_GetSysClockFreq();
    //PowerOn();

	MyLed_Configure();
	MyTimer_Configure();
    MyKey_Config();
	//MyUart_Configure();
	
	MyNVICConfigure();

    i = 0;
    while(1)
    {
        Delay( delay_time );
        if (i < 10)
        {
            data[i] = HAL_GetTick();
			i++;
        }
#if 0
				if (usart_tx_flag == 0)
		{
			usart_tx_flag = 1;
			//HAL_UART_Transmit_IT(&ghuart, (uint8_t *)data, sizeof(data));
			//HAL_UART_Transmit(&ghuart, (uint8_t *)data, sizeof(data), 5);
		}
        Delay( delay_time );
        HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET);
        Delay( delay_time );
        HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);

        Delay( delay_time );
        HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
        Delay( delay_time );
        HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
#endif
    }
}
