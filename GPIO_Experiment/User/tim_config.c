#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "m.h"

#define TIM6_PRESCALER  (9000 - 1)          // input 90MHz, after prescaler, 10KHz
#define TIM6_PERIOD     (10000 - 1)         // overflow each 1 seconds.
#define TIM2_PRESCALER  (9 - 1)             // input 90MHz, after prescaler, 10MHz
#define TIM2_PERIOD     (0xFFFFFFFF)        // in unit of 0.1us
#define TIM3_PRESCALLER (0)                 // cascade TIM2 and TIM3
#define TIM3_PERIOD     (0xFFFFFFFF)


#if 0
#define __HAL_RCC_TIM6_CLK_ENABLE()     do { \
                                        __IO uint32_t tmpreg = 0x00U; \
                                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        /* Delay after an RCC peripheral clock enabling */ \
                                        tmpreg = READ_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);\
                                        UNUSED(tmpreg); \
                                          } while(0)
#endif
/*
 * TIM8: GPIO_C_6
 *
 *
 */
static TIM_HandleTypeDef ghTim6;
static TIM_HandleTypeDef ghTim2;
static TIM_HandleTypeDef ghTim3;
static TIM_HandleTypeDef ghTim8;



/* is called by TIM6_DAC_IRQHandler */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// do something.
}

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
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 3);
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
    TIM_MasterConfigTypeDef sMasterConfig;
    // Configure TIM6 as basic timer.
    // Input clock freq: 90MHz
    // CLK_CNT (after prescaler): 10KHz
    ghTim6.Instance             = TIM6;
    ghTim6.State                = HAL_TIM_STATE_RESET;
    ghTim6.Init.CounterMode     = TIM_COUNTERMODE_UP;
    ghTim6.Init.ClockDivision   = TIM_CLOCKDIVISION_DIV1;
    ghTim6.Init.Period          = TIM6_PERIOD; // output 1Hz
    ghTim6.Init.Prescaler       = TIM6_PRESCALER;
    HAL_TIM_Base_Init(&ghTim6);

    /* TIM6 TRGO selection */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    HAL_TIM_Base_Start(&ghTim6);

    // disable TIM8 feature
    if (0)
    {
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
    }

    // Configure TIM2
    // Input clock freq: 90MHz
    // CLK_CNT (after prescaler): 90MHz / 9 = 10MHz
    //TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sOCConfig;
    TIM_SlaveConfigTypeDef sSlaveConfig;

    ghTim2.State                  = HAL_TIM_STATE_RESET;
    ghTim2.Instance               = TIM2;
    ghTim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
    ghTim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    ghTim2.Init.Period            = TIM2_PERIOD;
    ghTim2.Init.Prescaler         = TIM2_PRESCALER;
    ghTim2.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&ghTim2);

    // If not output signal and just cascade, don't need to set channel.
    if (0)
    {
        sOCConfig.OCMode = TIM_OCMODE_PWM1;
        sOCConfig.OCPolarity = TIM_OCPOLARITY_LOW;
        sOCConfig.OCIdleState = TIM_OCIDLESTATE_SET;
        sOCConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
        sOCConfig.Pulse = 31;
        HAL_TIM_PWM_ConfigChannel(&ghTim2, &sOCConfig, TIM_CHANNEL_1);
    }

    /* try to configure TIM2 as master clock */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    HAL_TIMEx_MasterConfigSynchronization(&ghTim2, &sMasterConfig);

    ghTim3.State                  = HAL_TIM_STATE_RESET;
    ghTim3.Instance               = TIM3;
    ghTim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    ghTim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    ghTim3.Init.Period            = TIM3_PERIOD;
    ghTim3.Init.Prescaler         = TIM3_PRESCALLER;
    ghTim3.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&ghTim3);

    // If not output signal and just cascade, don't need to set channel.
    if (0)
    {
        sOCConfig.OCMode = TIM_OCMODE_PWM1;
        sOCConfig.OCPolarity = TIM_OCPOLARITY_LOW;
        sOCConfig.Pulse = 1;
        HAL_TIM_PWM_ConfigChannel(&ghTim3, &sOCConfig, TIM_CHANNEL_1);
    }

    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
    sSlaveConfig.InputTrigger = TIM_TS_ITR1;
    HAL_TIM_SlaveConfigSynchronization(&ghTim3, &sSlaveConfig);

    HAL_TIM_PWM_Start(&ghTim3,  TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&ghTim2, TIM_CHANNEL_1);
}
