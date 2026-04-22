#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
#include "adc.h"
#include "task1.h"
#include "task2.h"
#include "task3.h"

#define PM_BAUD 9600

#define TASK_1 1
#define TASK_2 2
#define TASK_3 3

#define CURRENT_TASK TASK_2

int main()
{
  USART0_init(CALC_USART_UBRR(PM_BAUD));
  USART0_use_stdio();

  adc_init();

  sei();

  switch (CURRENT_TASK)
  {
  case TASK_1:
    task1();
    break;
  case TASK_2:
    task2();
    break;
  case TASK_3:
    task3();
    break;
  default:
    printf("Invalid task number\n");
    break;
  }

  return 0;
}
