#include <string.h>
#include "main.h"
#include "misc.h"
#include "queue_mgr.h"

#define DEFAULT_BAUD 115200

#define ENTRY_NUM 2
#define RECV_BUFFER_UNIT_SIZE 2048
#define SEND_BUFFER_UNIT_SIZE 2048

typedef enum _uart_mgr_state{
    WAIT_DATA_IN,
    WAIT_OUT_OUT
} UART_MGR_STATE;

typedef struct _recv_buff_ext{
    unsigned int data_size;
} RECV_BUFF_EXT;
typedef struct _send_buff_ext{
    unsigned int data_size;
} SEND_BUFF_EXT;

typedef struct _uart_mgr{
    unsigned char uart_recv_buff[RECV_BUFFER_UNIT_SIZE * ENTRY_NUM];
    unsigned char uart_send_buff[SEND_BUFFER_UNIT_SIZE * ENTRY_NUM];
    RECV_BUFF_EXT recv_buff_info[ENTRY_NUM];
    unsigned int send_buff_info[ENTRY_NUM];
    QUEUE_MGR uart_recv_mgr;
    QUEUE_MGR uart_send_mgr;
    volatile unsigned int send_interrupt_num;
    volatile unsigned int send_num;
    volatile unsigned int send_pending;
} UART_MGR, *PUART_MGR;

static UART_MGR uart_mgr;

static unsigned int total_recv_num_in_bytes = 0;
static unsigned int total_discard_num_in_bytes = 0;

void config_uart(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
    //---------------------串口功能配置---------------------
    //打开串口对应的外设时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
    //启动DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //DMA发送中断设置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //DMA1通道4配置
    DMA_DeInit(DMA1_Channel4);
    //外设地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_mgr.uart_send_buff;
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = 100;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
    //设置DMA的传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4,&DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);

    //使能通道4
    //DMA_Cmd(DMA1_Channel4, ENABLE);

    /**************************************************************************
     *
     * Serial DMA Receive
     *
     *************************************************************************/

    //DMA1通道5配置
    DMA_DeInit(DMA1_Channel5);
    //外设地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(uart_mgr.uart_recv_buff);
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = RECV_BUFFER_UNIT_SIZE;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //设置DMA的传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5,&DMA_InitStructure);

    //使能通道5
    DMA_Cmd(DMA1_Channel5,ENABLE);

    //初始化参数
    //USART_InitStructure.USART_BaudRate = DEFAULT_BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_BaudRate = DEFAULT_BAUD;
    //初始化串口
    USART_Init(USART1,&USART_InitStructure);
    //TXE发送中断,TC传输完成中断,RXNE接收中断,PE奇偶错误中断,可以是多个
    //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);

    //配置UART1中断
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;               //通道设置为串口1中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;       //中断占先等级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;              //中断响应优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //打开中断
    NVIC_Init(&NVIC_InitStructure);                                 //初始化

    //采用DMA方式发送
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
    //采用DMA方式Receive
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);

    //设置IO口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    //串口1的管脚初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                       //管脚9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;                //选择GPIO响应速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);                          //TX初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                      //管脚10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;                //选择GPIO响应速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);                          //RX初始化

    //设置IO口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                       //管脚9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;                //选择GPIO响应速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 //复用推挽输出

    //启动串口
    USART_Cmd(USART1, ENABLE);
}


//串口1DMA方式发送中断
//void DMA1_Channel4_IRQHandler(void)
void DMA1_Channel4_Event (uint32_t event)
{
    //清除标志位
    uart_mgr.send_interrupt_num++;
    DMA_ClearFlag(DMA1_FLAG_TC4);
    //DMA_ClearITPendingBit(DMA1_FLAG_TC4);
    //DMA1->IFCR |= DMA1_FLAG_TC4;
    //关闭DMA
    DMA_Cmd(DMA1_Channel4,DISABLE);

    queue_advance_consumer(&uart_mgr.uart_send_mgr, NULL);
    __DSB();
    //允许再次发送
    uart_mgr.send_pending = 0;
    __DSB();
}

//串口1接收中断
void USART1_IRQHandler(void)
{
    uint32_t temp = 0;
    unsigned int stage_size;
    unsigned int new_addr_for_recv;
    unsigned int *new_context;
    unsigned int buf_siz;


    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        temp = USART1->SR;
        temp = USART1->DR; //清USART_IT_IDLE标志
        DMA_Cmd(DMA1_Channel5,DISABLE);
        temp = DMA_GetCurrDataCounter(DMA1_Channel5);
        stage_size = (RECV_BUFFER_UNIT_SIZE - temp);
        if (queue_is_full(&uart_mgr.uart_recv_mgr))
        {
            // DATA MISSING! discard current data 
            total_discard_num_in_bytes += stage_size;
        }
        else
        {
            total_recv_num_in_bytes += stage_size;
            queue_advance_producer(&uart_mgr.uart_recv_mgr, &stage_size);
        }
        query_info_for_producer(&uart_mgr.uart_recv_mgr, &new_addr_for_recv, &buf_siz, (void*)&new_context);
        DMA1_Channel5->CMAR = (unsigned int)(new_addr_for_recv);
        DMA_SetCurrDataCounter(DMA1_Channel5, buf_siz);
        DMA_Cmd(DMA1_Channel5,ENABLE);
    }
}

static void recv_write_complete_cb(void* context, void* target_context)
{
    unsigned int* src = (unsigned int*)context;
    unsigned int* dest= (unsigned int*)target_context;
    *dest = *src;
}

static void recv_read_complete_cb(void* context, void* target_context)
{
    if (target_context)
    {
        unsigned int* dest= (unsigned int*)target_context;
        if (context)
        {
            unsigned int* src = (unsigned int*)context;
            *dest = *src;
        }
        else
        {
            *dest = 0;
        }
    }
}

void uart_init(void)
{
    uart_mgr.send_pending = 0;
    uart_mgr.send_num = 0;
    uart_mgr.send_interrupt_num = 0;
    queue_mgr_init(&uart_mgr.uart_recv_mgr, ENTRY_NUM,
        (unsigned int)uart_mgr.uart_recv_buff, RECV_BUFFER_UNIT_SIZE,
        (unsigned int)uart_mgr.recv_buff_info, sizeof(RECV_BUFF_EXT),
        recv_write_complete_cb, recv_read_complete_cb);

    queue_mgr_init(&uart_mgr.uart_send_mgr, ENTRY_NUM,
        (unsigned int)uart_mgr.uart_send_buff, SEND_BUFFER_UNIT_SIZE,
        (unsigned int)uart_mgr.send_buff_info, sizeof(SEND_BUFF_EXT),
        recv_write_complete_cb, recv_read_complete_cb);
    config_uart();
}

void uart_transform_handle(void)
{
    unsigned int recv_data_addr;
    unsigned int send_buff_addr;
    RECV_BUFF_EXT *recv_buff_ext;
    SEND_BUFF_EXT send_buff_ext;
    if ((queue_is_empty(&uart_mgr.uart_recv_mgr))
        || (queue_is_full(&uart_mgr.uart_send_mgr)))
    {
        return;
    }
    query_info_for_consumer(&uart_mgr.uart_recv_mgr, &recv_data_addr, NULL, (void*)&recv_buff_ext);
    query_info_for_producer(&uart_mgr.uart_send_mgr, &send_buff_addr, NULL, NULL);
    // transform frame from receiver to sender
    memcpy((void*)recv_data_addr, (void*)send_buff_addr, recv_buff_ext->data_size);
    send_buff_ext.data_size = recv_buff_ext->data_size;

    queue_advance_consumer(&uart_mgr.uart_recv_mgr, NULL);
    queue_advance_producer(&uart_mgr.uart_send_mgr, (void*)&send_buff_ext);
}
void uart_send_handle(void)
{
    unsigned int send_buff_addr;
    RECV_BUFF_EXT *send_buff_ext;

    if (uart_mgr.send_pending)
    {
        return;
    }
    if (queue_is_empty(&uart_mgr.uart_send_mgr))
    {
        return;
    }
    query_info_for_consumer(&uart_mgr.uart_send_mgr, &send_buff_addr, NULL, (void*)&send_buff_ext);
    //设置传输数据长度
    DMA_SetCurrDataCounter(DMA1_Channel4, send_buff_ext->data_size);
    //DMA1_Channel4->CMAR = send_buff_addr;
    uart_mgr.send_pending = 1;
    uart_mgr.send_num++;
		__DSB();
    //打开DMA
    DMA_Cmd(DMA1_Channel4, ENABLE);
}
