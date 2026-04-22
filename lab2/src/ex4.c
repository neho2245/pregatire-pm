#include "exenable.h"

#if NR_EX == 4


#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "usart.h"
#include "uptime.h"


#define LED_BLUE_PIN   PB3
#define LED_RED_PIN    PD5


static void GPIO_init(void)
{
    /* Blue LED */
    DDRB |= (1 << LED_BLUE_PIN);
    PORTB &= ~(1 << LED_BLUE_PIN);

    /* Red LED */
    DDRD |= (1 << LED_RED_PIN);
    PORTD &= ~(1 << LED_RED_PIN);
}



typedef void (*task_func_t)(void);

typedef struct
{
    uint32_t period_ms;
    uint32_t last_run_ms;
    task_func_t run;
} task_t;

/* Tasks */
static void task_heartbeat(void)
{
    PORTB ^= (1 << LED_BLUE_PIN);
}

static void task_logger(void)
{
    USART0_print("t=");
    USART0_print_u32(uptime_ms());
    USART0_print(" ms\r\n");
}

static void task_status(void)
{
    PORTD ^= (1 << LED_RED_PIN);
}


static void dispatcher(task_t *tasks, uint8_t task_count)
{
    uint32_t now = uptime_ms();

    for (uint8_t i = 0; i < task_count; i++) {
        if ((uint32_t)(now - tasks[i].last_run_ms) >= tasks[i].period_ms) {
            tasks[i].last_run_ms += tasks[i].period_ms;
            tasks[i].run();
        }
    }
}


int main(void)
{
    GPIO_init();
    USART0_init(9600);
    uptime_init();

    task_t tasks[] =
    {
        {1000,  0, task_heartbeat},
        {500,  0, task_logger},
        {3000, 0, task_status}
    };

    const uint8_t task_count = sizeof(tasks) / sizeof(tasks[0]);

    sei();

    while (1) {
        dispatcher(tasks, task_count);
    }
}



#endif