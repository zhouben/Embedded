//#include "stm32f4xx.h"
//#include "stm32f4xx_hal_gpio.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "./led/bsp_led.h"

#define DELAY_TIME_MIN  0x080000
#define DELAY_TIME      0x200000
#define DELAY_TIME_MAX  0x800000
#define DELAY_TIME_STEP 0x080000

static int32_t delay_time = DELAY_TIME;

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

static void LED_GPIO_Config(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启LED相关的GPIO外设时钟*/

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    //RCC_AHB1PeriphClockCmd ( LED1_GPIO_CLK|LED2_GPIO_CLK|LED3_GPIO_CLK|LED4_GPIO_CLK, ENABLE);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.Pin = LED1_PIN;

    /*设置引脚模式为输出模式*/
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;

    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.Alternate = GPIO_MODE_AF_PP;

    /*设置引脚为上拉模式*/
    GPIO_InitStructure.Pull = GPIO_PULLUP;

    /*设置引脚速率为2MHz */
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;

    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.Pin = LED2_PIN;
    HAL_GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.Pin = LED3_PIN;
    HAL_GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.Pin = LED4_PIN;
    HAL_GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*关闭RGB灯*/
    //LED_RGBOFF;

    /*指示灯默认开启*/
    //LED4(ON);
    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
}

static int led4_cnt = 0;

/*
 * interrupt handle for systick
 * default interrupt interval is 1ms, which is set in HAL_RCC_ClockConfig
 */
void SysTick_Handler(void)
{
    led4_cnt++;
    if(led4_cnt & 1)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    }else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    }
    if(led4_cnt == 500)
    {
        HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
    }
    else if(led4_cnt == 1000)
    {
        led4_cnt = 0;
        HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
    }
}

HAL_StatusTypeDef MyHSE_SetSysClock()
{
	RCC_OscInitTypeDef osc_init;
	HAL_StatusTypeDef ret;
	RCC_ClkInitTypeDef clk_init;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_ON;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	osc_init.PLL.PLLM = 25;
	osc_init.PLL.PLLN = 360;
	osc_init.PLL.PLLP = 2;
	osc_init.PLL.PLLQ = 7;
	ret = HAL_RCC_OscConfig(&osc_init);

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK;// | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
	ret = HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_10WS);
	return ret;
}

/*
 * Press Key1 to increase the period of LED blink
 */
void EXTI0_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/*
 * Press Key2 to decrease the period of LED blink
 */
void EXTI15_10_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_0)
	{
        delay_time += DELAY_TIME_STEP;
        if (delay_time > DELAY_TIME_MAX)
        {
            delay_time = DELAY_TIME_MAX;
        }
	}
	else
	{
        delay_time -= DELAY_TIME_STEP;
        if (delay_time < DELAY_TIME_MIN)
        {
            delay_time = DELAY_TIME_MIN;
        }
	}
}

static void Key_NVICConfigure()
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_1);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

static void Key_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	Key_NVICConfigure();
	__HAL_RCC_GPIOA_CLK_ENABLE(); // for Key1, GPIOA, Pin0
	__HAL_RCC_GPIOC_CLK_ENABLE(); // for Key2, GPIOC, Pin13
	
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.Pin = GPIO_PIN_0;

    /*设置引脚模式为输出模式*/
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;

    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.Alternate = GPIO_MODE_AF_PP;

    /*设置引脚为上拉模式*/
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    /*设置引脚速率为2MHz */
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;

    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_13;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static uint32_t freq = 0;

int main(void)
{
    MyHSE_SetSysClock();

    freq = HAL_RCC_GetSysClockFreq();

    if(freq != 0)
    {
        LED_GPIO_Config();
    }

	Key_Configure();
    HAL_NVIC_EnableIRQ(SysTick_IRQn);

    while(1)
    {
        Delay( delay_time );
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
        Delay( delay_time );
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);

        Delay( delay_time );
        HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET);
        Delay( delay_time );
        HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);

        Delay( delay_time );
        HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
        Delay( delay_time );
        HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
    }
}
