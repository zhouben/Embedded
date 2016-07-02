#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "m.h"

/*
 * interrupt handle for systick
 * default interrupt interval is 1ms, which is set in HAL_RCC_ClockConfig
 */
static int led1_cnt = 0;
static int cnt = 0;
static int tim6_cnt[10] = {0};
static int tim2_cnt[10] = {0};
static int tim3_cnt[10] = {0};

void SysTick_Handler(void)
{
    if (cnt < 10)
    {
        tim6_cnt[cnt] = Tim_GetTim6Cnt();
        tim2_cnt[cnt] = Tim_GetTim2Cnt();
        tim3_cnt[cnt] = Tim_GetTim3Cnt();
		cnt++;
    }
    led1_cnt++;
    if (led1_cnt == 500)
    {
        Led4_Blink();
    }
    else if (led1_cnt == 1000)
    {
        led1_cnt = 0;
        Led4_Blink();
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

	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
	ret = HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_10WS);
	return ret;
}
