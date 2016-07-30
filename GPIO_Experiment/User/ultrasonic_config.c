#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "m.h"

#define MEASURE_NUM_MAX 5
typedef enum ULTRASONIC_STATE_
{
    ULTRASONIC_READY,
    ULTRASONIC_WAIT_FOR_RESET_TRIGGER,
    ULTRASONIC_WAIT_FOR_ECHO_RISING,
    ULTRASONIC_WAIT_FOR_ECHO_FALLING,
    ULTRASONIC_FINAL
}ULTRASONIC_STATE;

typedef struct ULTRASONIC_CONTEXT_
{
    ULTRASONIC_STATE state;
    uint32_t cnt;
    uint32_t trigger_time[MEASURE_NUM_MAX];
    uint32_t rising_time[MEASURE_NUM_MAX];
    uint32_t falling_time[MEASURE_NUM_MAX];
    float    range[MEASURE_NUM_MAX];
}ULTRASONIC_CONTEXT;

ULTRASONIC_CONTEXT gUltrasonicContext;

void MyUltraSonic_Handle(ULTRASONIC_REQ_TYPE request)
{
    uint32_t currTime;
    switch(gUltrasonicContext.state)
    {
    case ULTRASONIC_READY:
        if (ULTRASONIC_REQ_RANGING_START == request)
        {
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET);
            gUltrasonicContext.trigger_time[gUltrasonicContext.cnt % MEASURE_NUM_MAX] = Tim_GetTim2Cnt();
            gUltrasonicContext.state = ULTRASONIC_WAIT_FOR_RESET_TRIGGER;
        }
        break;
    case ULTRASONIC_WAIT_FOR_RESET_TRIGGER:
        if (ULTRASONIC_REQ_RANGING_PROCESSING == request)
        {
            currTime = Tim_GetTim2Cnt();
            // > 20us
            if ((currTime - gUltrasonicContext.trigger_time[gUltrasonicContext.cnt % MEASURE_NUM_MAX]) > 200)
            {
                HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_RESET);
                gUltrasonicContext.state = ULTRASONIC_WAIT_FOR_ECHO_RISING;
            }
        }
        break;
    case ULTRASONIC_WAIT_FOR_ECHO_RISING:
        if (ULTRASONIC_REQ_ECHO_EVENT == request)
        {
            gUltrasonicContext.rising_time[gUltrasonicContext.cnt % MEASURE_NUM_MAX] = Tim_GetTim2Cnt();
            gUltrasonicContext.state = ULTRASONIC_WAIT_FOR_ECHO_FALLING;
        }
        break;
    case ULTRASONIC_WAIT_FOR_ECHO_FALLING:
        if (ULTRASONIC_REQ_ECHO_EVENT == request)
        {
            gUltrasonicContext.falling_time[gUltrasonicContext.cnt % MEASURE_NUM_MAX] = Tim_GetTim2Cnt();
            gUltrasonicContext.state = ULTRASONIC_FINAL;
        }
        break;
    case ULTRASONIC_FINAL:
        if (ULTRASONIC_REQ_RANGING_PROCESSING == request)
        {
            gUltrasonicContext.range[gUltrasonicContext.cnt % MEASURE_NUM_MAX] =
                (gUltrasonicContext.falling_time[gUltrasonicContext.cnt % MEASURE_NUM_MAX]
                 - gUltrasonicContext.rising_time[gUltrasonicContext.cnt % MEASURE_NUM_MAX]) * 340.0 / 1e7 / 2.0;
            gUltrasonicContext.cnt++;
            gUltrasonicContext.state = ULTRASONIC_READY;
        }
        break;
    default:
        break;
    }
}
/* Use GPIOH_2/3 as triger and echo signal
 */
void MyUltraSonic_Config(void)
{
	__HAL_RCC_GPIOH_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_2;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Alternate = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_3;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStructure.Alternate = 0;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
}
