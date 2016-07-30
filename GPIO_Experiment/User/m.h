#include "stm32f4xx_hal.h"

typedef enum ULTRASONIC_REQ_TYPE_
{
    ULTRASONIC_REQ_RANGING_START,
    ULTRASONIC_REQ_RANGING_PROCESSING,
    ULTRASONIC_REQ_ECHO_EVENT,
}ULTRASONIC_REQ_TYPE;

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
void MyUart_Configure(void);
void MyUltraSonic_Config(void);
void MyUltraSonic_Handle(ULTRASONIC_REQ_TYPE request);
