#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "m.h"

#define TIM6_PERIOD     0x09
#define __HAL_RCC_TIM6_CLK_ENABLE()     do { \
                                        __IO uint32_t tmpreg = 0x00U; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        UNUSED(tmpreg); \
                                          } while(0)
/*
 * TIM8: GPIO_C_6
 *
 *
 */
static TIM_HandleTypeDef ghTim6;
static TIM_HandleTypeDef ghTim2;
static TIM_HandleTypeDef ghTim3;
static TIM_HandleTypeDef ghTim8;

void TIM6_DAC_IRQHandler()
{
	HAL_TIM_IRQHandler(&ghTim6);
}

/* is called by TIM6_DAC_IRQHandler */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// do something.
}

#if 0
static void MyGPIO_Configure(void)
{
	/* for OC (PWM) output BKIN */
    GPIO_InitStructure.Pin = GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    /*
     * GPIOB: Pin13 (input)  -> rise interrupt EVTI13
     * connect PA3 to PB13 to let PA3 (in main loop) control PB13 to rise interrupt
     */
    GPIO_InitStructure.Pin = GPIO_PIN_13;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING; // GPIO_MODE_IT_RISING;
    //HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*
     * GPIOC: Pin13 (input)  -> for Key2 to riase interrupt for EVTI13
     */
    GPIO_InitStructure.Pin = GPIO_PIN_13;
    //HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}
#endif

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    switch((uint32_t)htim->Instance)
    {
    case (uint32_t)TIM2:
        __HAL_RCC_TIM2_CLK_ENABLE();
        break;
    case (uint32_t)TIM3:
        __HAL_RCC_TIM3_CLK_ENABLE();
        break;
    default:
        break;
    }
}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
    switch((uint32_t)htim->Instance)
    {
    case (uint32_t)TIM8:
        __HAL_RCC_TIM8_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        GPIO_InitStructure.Pin = GPIO_PIN_6;
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStructure.Alternate = GPIO_AF3_TIM8;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
        break;
    default:
        break;
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    switch((uint32_t)htim->Instance)
    {
    case (uint32_t)TIM6:
        __HAL_RCC_TIM6_CLK_ENABLE();
        break;
    default:
        break;
    }
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

int Tim_GetTim6Cnt(void)
{
    return __HAL_TIM_GET_COUNTER(&ghTim6);
}

int Tim_GetTim2Cnt(void)
{
    return __HAL_TIM_GET_COUNTER(&ghTim2);
}

int Tim_GetTim3Cnt(void)
{
    return __HAL_TIM_GET_COUNTER(&ghTim3);
}
/*
 * TIM6: basic timer, in units of 100us
 * TIM8: 
 * Configure TIM6 as basic timer
 * Once it expires, TIM6_DAC_IRQHandler will be called when TIM6_DAC_IRQn is enabled.
 */
void MyTimer_Configure(void)
{
    // configure TIM6 as basic timer.
    // Input clock freq: 90MHz
    // CLK_CNT (after prescaler): 10KHz
	ghTim6.Instance = TIM6;
	ghTim6.State = HAL_TIM_STATE_RESET;
	ghTim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	ghTim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	ghTim6.Init.Period = 0xFFFF;
	ghTim6.Init.Prescaler = 9000 - 1;
	HAL_TIM_Base_Init(&ghTim6);
	
    // configure TIM8 as PWM output
	TIM_OC_InitTypeDef oc_init;
	ghTim8.State = HAL_TIM_STATE_RESET;
	ghTim8.Instance = TIM8;
	ghTim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	ghTim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	ghTim8.Init.Period = 1024 - 1;
	ghTim8.Init.Prescaler = 180 - 1;
	ghTim8.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&ghTim8);
	
	oc_init.OCMode = TIM_OCMODE_PWM1; // ; TIM_OCMODE_ACTIVE error!
	oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
	oc_init.OCNPolarity = TIM_OCNPOLARITY_HIGH; //TIM_OCNPOLARITY_HIGH;
	oc_init.OCIdleState = TIM_OCIDLESTATE_SET;
	oc_init.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	oc_init.Pulse = 256; // 2.56ms
	//oc_init.OCFastMode = TIM_CCMR1_OC1PE; // TIM_OCFAST_ENABLE;
	HAL_TIM_OC_ConfigChannel(&ghTim8, &oc_init, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&ghTim8, TIM_CHANNEL_1);
	//HAL_TIM_PWM_ConfigChannel(&ghTim8, &oc_init, TIM_CHANNEL_1);
	//HAL_TIM_PWM_Start(&ghTim8, TIM_CHANNEL_1);
	
    // configure TIM2
    // Input clock freq: 90MHz
    // CLK_CNT (after prescaler): 90MHz / 9 = 10MHz
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sOCConfig;
    TIM_SlaveConfigTypeDef sSlaveConfig;

	ghTim2.State = HAL_TIM_STATE_RESET;
	ghTim2.Instance = TIM2;
	ghTim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	ghTim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	ghTim2.Init.Period = 25000 - 1; //9999999; // 10^7 - 1, overflow every 1 second.
	ghTim2.Init.Prescaler = 8;
	ghTim2.Init.RepetitionCounter = 0;
	HAL_TIM_PWM_Init(&ghTim2);

    if (1)
    {
        sOCConfig.OCMode = TIM_OCMODE_PWM1;
        sOCConfig.OCPolarity = TIM_OCPOLARITY_LOW;
        sOCConfig.OCIdleState = TIM_OCIDLESTATE_SET;
        sOCConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
        sOCConfig.Pulse = 31;
        HAL_TIM_PWM_ConfigChannel(&ghTim2, &sOCConfig, TIM_CHANNEL_1);

        sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;

    /* try to configure TIM2 as master clock */
        HAL_TIMEx_MasterConfigSynchronization(&ghTim2, &sMasterConfig);

        ghTim3.State = HAL_TIM_STATE_RESET;
        ghTim3.Instance = TIM3;
        ghTim3.Init.CounterMode = TIM_COUNTERMODE_UP;
        ghTim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        ghTim3.Init.Period = 3;
        ghTim3.Init.Prescaler = 0;
        ghTim3.Init.RepetitionCounter = 0;
        HAL_TIM_PWM_Init(&ghTim3);

        sOCConfig.OCMode = TIM_OCMODE_PWM1;
        sOCConfig.OCPolarity = TIM_OCPOLARITY_LOW;
        sOCConfig.Pulse = 1;
        HAL_TIM_PWM_ConfigChannel(&ghTim3, &sOCConfig, TIM_CHANNEL_1);

        sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
        sSlaveConfig.InputTrigger = TIM_TS_ITR1;

        HAL_TIM_SlaveConfigSynchronization(&ghTim3, &sSlaveConfig);
    }

    HAL_TIM_PWM_Start(&ghTim3,  TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&ghTim2, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&ghTim6);
}
