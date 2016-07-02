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
