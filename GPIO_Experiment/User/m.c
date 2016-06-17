//#include "stm32f4xx.h"
//#include "stm32f4xx_hal_gpio.h"

#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "./led/bsp_led.h"

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

    /*关闭RGB灯*/
    //LED_RGBOFF;

    /*指示灯默认开启*/
    //LED4(ON);
		HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
}

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
#define DELAY_TIME 0x2FFFFF
int main(void)
{
	LED_GPIO_Config();
	while(1)
	{
		Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
		Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED4_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
		
				Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
		Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
		
				Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET);
		Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
		
				Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
		Delay( DELAY_TIME );
		HAL_GPIO_WritePin(LED3_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
	}
}
