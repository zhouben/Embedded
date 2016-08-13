#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_i2c.h"
#include "m.h"

#define I2C_ADDRESS_OWN 0xB0
#define I2C_ADDRESS_EEPROM 0xA0

/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor I2Cx/I2Cx instance used and associated
   resources */
/* Definition for I2Cx clock resources */
#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_6
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_7
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1

/* Definition for I2Cx's NVIC */
#define I2Cx_EV_IRQn                    I2C1_EV_IRQn
#define I2Cx_EV_IRQHandler              I2C1_EV_IRQHandler
#define I2Cx_ER_IRQn                    I2C1_ER_IRQn
#define I2Cx_ER_IRQHandler              I2C1_ER_IRQHandler

/* ---------------- end of from sample ----------------*/

I2C_HandleTypeDef I2cHandle;

void MyI2c_EepromWrite(uint8_t *aTxBuffer, uint16_t size)
{
    /* The board sends the message and expects to receive it back */
    do
    {
        /*##-2- Start the transmission process #####################################*/
        /* While the I2C in reception process, user can transmit data through
           "aTxBuffer" buffer */
        if(HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)I2C_ADDRESS_EEPROM, (uint8_t*)aTxBuffer, size)!= HAL_OK)
        {
            /* Error_Handler() function is called in case of error. */
            Error_Handler();
        }

        /*##-3- Wait for the end of the transfer ###################################*/
        /*  Before starting a new communication transfer, you need to check the current
            state of the peripheral; if it抯 busy you need to wait for the end of current
            transfer before starting a new one.
            For simplicity reasons, this example is just waiting till the end of the
            transfer, but application may perform other tasks while transfer operation
            is ongoing. */
        while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
        {
        }

        /* When Acknowledge failure occurs (Slave don't acknowledge its address)
           Master restarts communication */
    }
    while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);
}

void MyI2c_EepromWriteSample()
{
    //uint8_t str[] = " hello world! zhouben";
    uint8_t str[] = " abcdef78";
    str[0] = 8;
    MyI2c_EepromWrite(str, 9);
}

/*
 * read an amount of data in interrupt mode.
 */
void MyI2c_EepromRead(uint8_t *aRxBuffer, uint16_t buff_size)
{
    /*##-4- Put I2C peripheral in reception process ############################*/
    do
    {
        if(HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)I2C_ADDRESS_EEPROM, (uint8_t *)aRxBuffer, buff_size) != HAL_OK)
        {
            /* Error_Handler() function is called in case of error. */
            Error_Handler();
        }

        /* When Acknowledge failure occurs (Slave don't acknowledge its address)
           Master restarts communication */
    }
    while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

    // need to wait for read completed.
    while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
    {
    } 
}

/*
 * Interrupt read
 */
void MyI2c_EepromReadSample()
{
    uint8_t data[2];
    uint8_t str[16] = {0};
    data[0] = 0;
    MyI2c_EepromWrite(data, 1);
    // wait about 10us
    Delay(140);
    MyI2c_EepromRead(str, 16);
}

/* ---------- Polling Mode -----------------*/

void MyI2c_EepromPollingReadSample()
{
    uint8_t data[2];
    uint8_t str[16] = {0};
    data[0] = 8;
    //MyI2c_EepromWrite(data, 1);

    while(HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)I2C_ADDRESS_EEPROM, (uint8_t*)data, 1, 10000)!= HAL_OK)
    {
        /* Error_Handler() function is called when Timeout error occurs.
           When Acknowledge failure occurs (Slave don't acknowledge it's address)
           Master restarts communication */
        if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
        {
            Error_Handler();
        }
    }

    Delay(140);
    //MyI2c_EepromRead(str, 8);
    while(HAL_I2C_Master_Receive(&I2cHandle, (uint16_t)I2C_ADDRESS_EEPROM, (uint8_t *)str, 16, 10000) != HAL_OK)
    {
        /* Error_Handler() function is called when Timeout error occurs.
           When Acknowledge failure occurs (Slave don't acknowledge it's address)
           Master restarts communication */
        if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
        {
            Error_Handler();
        }   
    }
}
void MyI2c_EepromConfig()
{
    /*##-1- Configure the I2C peripheral ######################################*/
    I2cHandle.Instance             = I2Cx;

    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.ClockSpeed      = 400000;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
    I2cHandle.Init.OwnAddress1     = I2C_ADDRESS_OWN;
    I2cHandle.Init.OwnAddress2     = 0xFE;

    if(HAL_I2C_Init(&I2cHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}

/**
  * @brief I2C MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*##-1- Enable GPIO Clocks #################################################*/
    /* Enable GPIO TX/RX clock */
    I2Cx_SCL_GPIO_CLK_ENABLE();
    I2Cx_SDA_GPIO_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* I2C TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = I2Cx_SCL_AF;

    HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

    /* I2C RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
    GPIO_InitStruct.Alternate = I2Cx_SDA_AF;

    HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

    /*##-3- Enable I2C peripheral Clock ########################################*/
    /* Enable I2C1 clock */
    I2Cx_CLK_ENABLE();

    /*##-4- Configure the NVIC for I2C #########################################*/
    /* NVIC for I2C1 */
    HAL_NVIC_SetPriority(I2Cx_ER_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(I2Cx_ER_IRQn);
    HAL_NVIC_SetPriority(I2Cx_EV_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(I2Cx_EV_IRQn);
}

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to I2C data transmission
  */
void I2Cx_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(&I2cHandle);
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to I2C error
  */
void I2Cx_ER_IRQHandler(void)
{
    HAL_I2C_ER_IRQHandler(&I2cHandle);
}
