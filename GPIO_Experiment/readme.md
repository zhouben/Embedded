功能介绍

# 超声波测距模块 HC-SR04 的控制
1. 按照以下链接方式连接STM32F4开发板与超声波模块的管脚
5V  -- VCC
PH2 -- Trigger
PH3 -- Echo
GND -- GND
1. 按照以下流程控制超声波模块
    1. SysTick\_Handle里边启动测距，即设置 GPIOH\_PIN2为高电平
    2. main loop里边等待至少10us以后将 GPIOH\_PIN2设置为低电平
    3. GPIOH\_PIN3设置为输入+中断触发模式（上升沿和下降沿）
        1. 在PH3的上升沿记下当前TIM2，保存在 rising_time
        1. 在PH3的下降沿记下当前TIM2，保存在 falling_time
    4. 在 main loop里边做最后的计算，然后将内部测距模块的状态恢复到初始状态
1. 测距模块要求输入为TTL电平。实际 PH2 and PH3均为3.3V的电平。


