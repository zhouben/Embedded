#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_dac.h"
#include "m.h"

DAC_HandleTypeDef ghDac1;
static uint8_t dac_index = 0;
static uint16_t dac_data[] = {
2448,2832,3186,3496,3751,3940,4057,4095,4057,3940,3751,3496,3186,2832,2448,2048,1648,1264,910,600,345,156,39,0,39,156,345,600,910,1264,1648,2048
};
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_DAC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE(); // for Key1, GPIOA, Pin0
    __HAL_RCC_DMA1_CLK_ENABLE();
	/* configure PD5/6 for UART2 */
	GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStructure.Alternate = 0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);


}
void MyDAC_Config(void)
{
    DAC_ChannelConfTypeDef config;
    ghDac1.State = HAL_DAC_STATE_RESET;
    ghDac1.Instance = DAC;
    HAL_DAC_Init(&ghDac1);

    config.DAC_Trigger = DAC_TRIGGER_NONE; // DAC_TRIGGER_T6_TRGO;
    //config.DAC_WaveGeneration = 0;
    config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE; //DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&ghDac1, &config, DAC_CHANNEL_1);
    HAL_DAC_Start(&ghDac1, DAC_CHANNEL_1);
}

void MyDac_Update()
{
    HAL_DAC_SetValue(&ghDac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_data[(dac_index++) & 0x1F]);
}
