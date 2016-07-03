#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_dac.h"
#include "m.h"

DAC_HandleTypeDef ghDac1;
DMA_HandleTypeDef ghDma;
static uint8_t dac_index = 0;
static uint32_t dac_data_length = 32;
static uint32_t dac_data[] = {
2448,2832,3186,3496,3751,3940,4057,4095,4057,3940,3751,3496,3186,2832,2448,2048,1648,1264,910,600,345,156,39,0,39,156,345,600,910,1264,1648,2048
};

void TIM6_DAC_IRQHandler()
{
	//HAL_TIM_IRQHandler(&ghTim6);
	//HAL_DAC_IRQHandler(&ghDac1);
}

void DMA1_Stream5_IRQHandler(void)
{
	//HAL_DAC_IRQHandler(ghDac1.DMA_Handle1);
	HAL_DMA_IRQHandler(ghDac1.DMA_Handle1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // for Key1, GPIOA, Pin0
    //__HAL_RCC_DMA2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    /* configure PD5/6 for UART2 */
    GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStructure.Alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    ghDma.Instance = DMA1_Stream5;
//    ghDma.StreamBaeAddress = dac_data  ??;
    ghDma.Init.Channel = DMA_CHANNEL_7;
    ghDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    ghDma.Init.PeriphInc = DMA_PINC_DISABLE;
    ghDma.Init.MemInc = DMA_MINC_ENABLE;
    ghDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    ghDma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    ghDma.Init.Mode = DMA_CIRCULAR;
    ghDma.Init.Priority = DMA_PRIORITY_HIGH;
    ghDma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
    ghDma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    ghDma.Init.MemBurst = DMA_MBURST_SINGLE;
    ghDma.Init.PeriphBurst = DMA_PBURST_SINGLE; 

    HAL_DMA_Init(&ghDma);

    /* Associate the initialized DMA handle to the the DAC handle */
    __HAL_LINKDMA(hdac, DMA_Handle1, ghDma);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* Enable the DMA1 Stream5 IRQ Channel */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 1, 2);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
    dac_index++;
}
void DACx_DMA_IRQHandle1(void)
{
    HAL_DMA_IRQHandler((ghDac1.DMA_Handle1));
}
void DACx_DMA_IRQHandle2(void)
{
    HAL_DMA_IRQHandler((ghDac1.DMA_Handle2));
}

void MyDAC_Config(uint32_t DMAMode)
{
    DAC_ChannelConfTypeDef config;
    ghDac1.State = HAL_DAC_STATE_RESET;
    ghDac1.Instance = DAC;
    HAL_DAC_Init(&ghDac1);

    config.DAC_Trigger = DAC_TRIGGER_T6_TRGO;// DAC_TRIGGER_NONE; // DAC_TRIGGER_T6_TRGO;
    //config.DAC_WaveGeneration = 0;
    config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE; //DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&ghDac1, &config, DAC_CHANNEL_1);
    if (DMAMode == 1)
    {
        HAL_DAC_Start_DMA(&ghDac1, DAC_CHANNEL_1, dac_data, dac_data_length, DAC_ALIGN_12B_R);
    }
    else
    {
        HAL_DAC_Start(&ghDac1, DAC_CHANNEL_1);
    }
}

void MyDac_DMAStart(uint32_t *data, uint32_t length, uint32_t alignment)
{
    HAL_DAC_Start_DMA(&ghDac1, DAC_CHANNEL_1, data, length, alignment);
}

void MyDac_Update()
{
    HAL_DAC_SetValue(&ghDac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_data[(dac_index++) & 0x1F]);
}
