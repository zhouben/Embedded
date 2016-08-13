#ifndef _M_H_
#define _M_H_
#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal.h"

void Led4_Blink(void);
void MyKey_Config(void);
void MyLed_Configure(void);
HAL_StatusTypeDef MyHSE_SetSysClock(void);
void MyNVICConfigure(void);
int Tim_GetTim6Cnt(void);
int Tim_GetTim2Cnt(void);
int Tim_GetTim3Cnt(void);
void MyTimer_Configure(void);
int MyUsart_SendDataSync(uint8_t *data, int siz);
void MyDac_Update(void);
void MyDAC_Config(uint32_t dma_mode);
void Error_Handler(void);
void MyUart_Configure(void);

void MyI2c_EepromConfig(void);
void MyI2c_EepromWriteSample(void);
void MyI2c_EepromReadSample(void);
void MyI2c_EepromPollingReadSample(void);

void Delay(__IO uint32_t nCount);     //简单的延时函数
#endif
