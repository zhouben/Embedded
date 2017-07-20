#include "main.h"

int main(void)
{
    gpio_blink_init();
    uart_init();

    while(1)
    {
        sbas_crc24_test();
        uart_transform_handle();
        uart_send_handle();
        gpio_blink_handle(1);
    }
}
