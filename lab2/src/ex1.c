#include "exenable.h"


#if  NR_EX == 1


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#include "usart.h"
#include "uptime.h"



int main(void)
{
    //GPIO_init();
    USART0_init(9600);
    uptime_init();

    sei();

    uint32_t last_msg = 0;

    while (1) {
        uint32_t now = uptime_ms();

        if ((uint32_t)(now - last_msg) >= 1000) {
            last_msg += 1000;

            USART0_print("Uptime: ");
            USART0_print_u32(now);
            USART0_print(" ms\r\n");
        }
    }
}

#endif