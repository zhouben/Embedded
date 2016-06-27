//#include "stm32f4xx.h"
//#include "stm32f4xx_hal_gpio.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "./led/bsp_led.h"

#define DELAY_TIME_MIN  0x080000
#define DELAY_TIME      0x200000	// about 150ms
#define DELAY_TIME_MAX  0x800000
#define DELAY_TIME_STEP 0x080000

#define TIM6_PERIOD     0x09

#define __HAL_RCC_TIM6_CLK_ENABLE()     do { \
                                        __IO uint32_t tmpreg = 0x00U; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        UNUSED(tmpreg); \
                                          } while(0)

static int32_t delay_time = DELAY_TIME;
static TIM_HandleTypeDef gHtim;

/* for nCount = 0x20_0000, delay time is around 150ms */
static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}

static void PowerOn(void)
{
    /*开启LED相关的GPIO外设时钟*/
	__HAL_RCC_GPIOA_CLK_ENABLE(); // for Key1, GPIOA, Pin0
	__HAL_RCC_GPIOC_CLK_ENABLE(); // for Key2, GPIOC, Pin13
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    __HAL_RCC_TIM6_CLK_ENABLE();
	__HAL_RCC_TIM8_CLK_ENABLE();
}
static void MyGPIO_Configure(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.Pin = LED1_PIN | LED2_PIN | LED3_PIN;
    /*设置引脚模式为输出模式*/
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.Alternate = GPIO_MODE_AF_PP;
    /*设置引脚为上拉模式*/
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    /*设置引脚速率为2MHz */
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    /*调用库函数，使用上面配置的GPIO_InitStructure初始化GPIO*/
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.Pin = LED4_PIN;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

    HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);

    /*
     * GPIOA: Pin0 (input)  -> Key1
     *        Pin3 (output) -> to control peripheral
     */
    GPIO_InitStructure.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* for OC (PWM) output BKIN */
    GPIO_InitStructure.Pin = GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    /*
     * GPIOB: Pin13 (input)  -> rise interrupt EVTI13
     * connect PA3 to PB13 to let PA3 (in main loop) control PB13 to rise interrupt
     */
    GPIO_InitStructure.Pin = GPIO_PIN_13;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING; // GPIO_MODE_IT_RISING;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*
     * GPIOC: Pin13 (input)  -> for Key2 to riase interrupt for EVTI13
     */
    GPIO_InitStructure.Pin = GPIO_PIN_13;
    //HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF3_TIM8;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
}

static int led4_cnt = 0;
void Blink_LED(void)
{
    led4_cnt++;
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

/*
 * interrupt handle for systick
 * default interrupt interval is 1ms, which is set in HAL_RCC_ClockConfig
 */
static int led1_cnt = 0;
void SysTick_Handler(void)
{
    led1_cnt++;
    if (led1_cnt == 500)
    {
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
    }
    else if (led1_cnt == 1000)
    {
        led1_cnt = 0;
        HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
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

void TIM6_DAC_IRQHandler()
{
	HAL_TIM_IRQHandler(&gHtim);
}

/* is called by TIM6_DAC_IRQHandler */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//Blink_LED();
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
}

/*
 * Set NVIC for Key1, Key2 and TIM6
 */
static void MyNVICConfigure()
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_1);

	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

/*
 * Configure TIM6 as basic timer
 * period is 1ms
 * Once it expires, TIM6_DAC_IRQHandler will be called when TIM6_DAC_IRQn is enabled.
 */
static void MyTimer_Configure()
{
	gHtim.Instance = TIM6;
	gHtim.State = HAL_TIM_STATE_RESET;
	gHtim.Init.CounterMode = TIM_COUNTERMODE_UP;
	gHtim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	gHtim.Init.Period = TIM6_PERIOD;
	gHtim.Init.Prescaler = 9000 - 1;

	HAL_TIM_Base_Init(&gHtim);
	//HAL_TIM_Base_Start_IT(&gHtim);
	
	TIM_OC_InitTypeDef oc_init;
    TIM_HandleTypeDef htim;

	htim.State = HAL_TIM_STATE_RESET;
	htim.Instance = TIM8;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.Period = 1024 - 1;
	htim.Init.Prescaler = 1800 - 1;
	htim.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&htim);
	
	oc_init.OCMode = TIM_OCMODE_PWM1;
	oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
	oc_init.OCNPolarity = TIM_OCNPOLARITY_LOW; //TIM_OCNPOLARITY_HIGH;
	oc_init.OCIdleState = TIM_OCIDLESTATE_SET;
	oc_init.OCNIdleState = TIM_OCNIDLESTATE_SET;
	oc_init.Pulse = 372;
	//oc_init.OCFastMode = TIM_CCMR1_OC1PE; // TIM_OCFAST_ENABLE;
	HAL_TIM_PWM_ConfigChannel(&htim, &oc_init, TIM_CHANNEL_1);
	//TIM_OC1_SetConfig(TIM8, &oc_init);
	HAL_TIM_OC_Start(&htim, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start_IT(&htim, 

    HAL_TIM_Base_Start_IT(&gHtim);
}

static uint32_t freq = 0;

int main(void)
{
    MyHSE_SetSysClock();

    freq = HAL_RCC_GetSysClockFreq();
    PowerOn();

	MyGPIO_Configure();
	MyTimer_Configure();
	
	MyNVICConfigure();

    while(1)
    {
        Delay( delay_time );
        //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
        Delay( delay_time );
        //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
#if 0
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
