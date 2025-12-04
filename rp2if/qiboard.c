#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

int main()
{
    stdio_init_all();
    adc_init();
    
    adc_gpio_init(26);
    adc_select_input(0);

    while (1)
    {
        uint16_t res=adc_read();
        printf("%d\n", res);
        sleep_ms(144);
    }

    return 0;
}

