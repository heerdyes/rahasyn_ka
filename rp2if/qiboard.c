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
    
    // averaging filter
    uint16_t a1=0,a2=0,a3=0,a4=0;

    while (1)
    {
        a4=adc_read();
        
        uint16_t avg=(a1+a2+a3+a4)/4;
        printf("%d\n", avg);
        
        a1=a2;
        a2=a3;
        a3=a4;
        
        sleep_ms(99);
    }

    return 0;
}

