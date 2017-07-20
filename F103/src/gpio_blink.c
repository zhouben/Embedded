#include "main.h"

static int cnt = 0;
static int last_state = 0;

void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 
    //=============================================================================
    //LED -> PC13
    //=============================================================================			 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void gpio_blink_init(void)
{
    GPIO_Configuration();
}
void gpio_blink_handle(uint32_t enable)
{
    cnt++;
    if (cnt == 1023)
    {
        cnt = 0;
        last_state = (last_state) ? 0 : 1;
        if (enable)
        {
            PCout(13)= (last_state) ? 1 : 0;
        }
    }
}
